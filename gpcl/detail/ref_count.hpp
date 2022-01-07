//
// ref_count.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_REF_COUNT_HPP
#define GPCL_DETAIL_REF_COUNT_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/ref_count_base.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/unique_ptr.hpp>

#include <memory>
#include <utility>

namespace gpcl {
namespace detail {

template <typename Ptr, typename Deleter, typename Allocator>
class ref_count : public ref_count_base,
                  private compressed_pair<Ptr, Deleter>,
                  private Allocator
{
private:
  using rebind_allocator = typename std::allocator_traits<
      Allocator>::template rebind_alloc<ref_count>;
  using rebind_allocator_traits = std::allocator_traits<rebind_allocator>;

  /// destroy the ref_count itself then deallocate the memory.
  inline void delete_this() noexcept
  {
    GPCL_ASSERT_CONST(this->use_count() == 0);
    GPCL_ASSERT_CONST(this->weak_count() == 0);
    rebind_allocator alloc{*this};
    this->~ref_count();
    rebind_allocator_traits::deallocate(alloc, this, 1);
  }

  /// operation function.
  static void *do_operate(ref_count_base *self,
                          ref_count_operation_t op) noexcept
  {
    auto s = static_cast<ref_count *>(self);
    switch (op)
    {
    case delete_control_block:
      s->delete_this();
      break;

    case destroy_managed_object:
      s->delete_managed_object();
      break;

    case ref_count_operation_t::get_deleter:
      return s->get_deleter();

#if !defined GPCL_NO_RTTI
    case ref_count_operation_t::get_deleter_type_info:
      return s->get_deleter_type_info();
#endif

    default:
      GPCL_UNREACHABLE("invalid ref_count operation");
    }

    return nullptr;
  }

public:
  //// Create a ref_count.
  /// @param allocator allocator to allocate memory for the created ref_count.
  /// @param args arguments passed to the constructor of Manager.
  template <typename... Args>
  [[nodiscard]] static ref_count *create(const Allocator &allocator,
                                         Args &&...args)
  {
    rebind_allocator alloc{allocator};

    unique_ptr<void, allocator_delete> addr(
        rebind_allocator_traits::allocate(alloc, 1), allocator_delete{alloc});

    auto r = ::new (addr.get())
        ref_count(&do_operate, allocator, std::forward<Args>(args)...);
    (void)addr.release();
    return r;
  }

private:
  struct allocator_delete
  {
    rebind_allocator &alloc_;

    void operator()(void *p) noexcept
    {
      rebind_allocator_traits::deallocate(alloc_,
                                          reinterpret_cast<ref_count *>(p), 1);
    }
  };

public:
  /// Delete the managed object.
  inline void delete_managed_object() noexcept
  {
    GPCL_ASSERT_CONST(this->use_count() == 0);
    if (this->first())
    {
      this->second()(this->first());
      this->first() = Ptr();
    }
  }

  inline Deleter *get_deleter() noexcept { return &this->second(); }

#if !defined GPCL_NO_RTTI
  static inline std::type_info *get_deleter_type_info() noexcept
  {
    return const_cast<std::type_info *>(std::addressof(typeid(Deleter)));
  }
#endif

public:
  ref_count(ref_count_base::operation_func_t op_func, Allocator a, Ptr p,
            Deleter d)
#if defined GPCL_NO_EXCEPTIONS
      : ref_count_base(op_func),
        compressed_pair<Ptr, Deleter>(p, std::move(d)),
        Allocator(a)
  {
  }
#else
  try : ref_count_base(op_func), compressed_pair
    <Ptr, Deleter>(p, std::move_if_noexcept(d)), Allocator(a) {}
  catch (...)
  {
    d(p);
    throw;
  }
#endif

  inline ~ref_count() noexcept { delete_managed_object(); }
};

template <typename Ptr, typename Deleter, typename Allocator>
using ref_count_ptr = ref_count<Ptr, Deleter, Allocator>;

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_REF_COUNT_HPP
