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

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/ref_count_base.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/typeid.hpp>
#include <gpcl/unique_ptr.hpp>

#include <memory>
#include <utility>

namespace gpcl {
namespace detail {

template <typename Derived, typename Allocator>
class ref_count : public ref_count_base, public Allocator
{
private:
  using rebind_allocator =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Derived>;
  using rebind_allocator_traits = std::allocator_traits<rebind_allocator>;

  /// destroy the ref_count itself then deallocate the memory.
  inline void delete_this() noexcept
  {
    GPCL_ASSERT(this->use_count() == 0);
    GPCL_ASSERT(this->weak_count() == 0);
    rebind_allocator alloc{*this};
    auto p = static_cast<Derived *>(this);
    p->~Derived();
    rebind_allocator_traits::deallocate(alloc, p, 1);
  }

protected:
  /// operation function.
  static void *do_operate(ref_count_base *self,
                          ref_count_operation_t op) noexcept
  {
    auto s = static_cast<Derived *>(self);
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

    case ref_count_operation_t::get_deleter_type_info:
      return s->get_deleter_type_info();

    default:
      GPCL_UNREACHABLE("invalid ref_count operation");
    }

    return nullptr;
  }

  /// Constructor.
  /// @param op_func the operation function, normally &ref_count::do_operate.
  /// @param a allocator
  template <typename... Args>
  explicit ref_count(operation_func_t op_func, Allocator a)
      : ref_count_base{op_func},
        Allocator(a)
  {
  }

public:
  //// Create a ref_count.
  /// @param allocator allocator to allocate memory for the created ref_count.
  /// @param args arguments passed to the constructor of Manager.
  template <typename... Args>
  [[nodiscard]] static Derived *create(const Allocator &allocator,
                                       Args &&...args)
  {
    rebind_allocator alloc{allocator};

    auto *addr = rebind_allocator_traits::allocate(alloc, 1);

    GPCL_TRY
    {
      auto r = ::new (addr)
          Derived(&do_operate, allocator, std::forward<Args>(args)...);
      return r;
    }
    GPCL_CATCH(...)
    {
      rebind_allocator_traits::deallocate(alloc, addr, 1);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

private:
  struct allocator_delete
  {
    rebind_allocator &alloc_;

    void operator()(void *p) noexcept
    {
      rebind_allocator_traits::deallocate(alloc_,
                                          reinterpret_cast<Derived *>(p), 1);
    }
  };
};

template <typename Ptr, typename Deleter, typename Allocator>
class ref_count_ptr
    : public ref_count<ref_count_ptr<Ptr, Deleter, Allocator>, Allocator>
{
  using base_type =
      ref_count<ref_count_ptr<Ptr, Deleter, Allocator>, Allocator>;

  compressed_pair<Ptr, Deleter> p_;

public:
  /// Delete the managed object.
  inline void delete_managed_object() noexcept
  {
    GPCL_ASSERT(base_type::use_count() == 0);
    if (p_.first())
    {
      p_.second()(p_.first());
      p_.first() = Ptr();
    }
  }

  inline Deleter *get_deleter() noexcept { return &p_.second(); }

  static inline type_info *get_deleter_type_info() noexcept
  {
    return const_cast<type_info *>(std::addressof(typeid_<Deleter>()));
  }

public:
  explicit inline ref_count_ptr(ref_count_base::operation_func_t op_func,
                                Allocator a, Ptr p, Deleter d)
#if defined GPCL_CONFIG_NO_EXCEPTIONS
      : base_type(op_func, a),
        p_(p, std::move(d))
  {
  }
#else
  try : base_type(op_func, a), p_(p, std::move(d))
  {
  }
  catch (...)
  {
    d(p);
    throw;
  }
#endif

  inline ~ref_count_ptr() noexcept { delete_managed_object(); }
};

template <typename T, typename Allocator>
class ref_count_obj : public ref_count<ref_count_obj<T, Allocator>, Allocator>
{
  using base_type = ref_count<ref_count_obj<T, Allocator>, Allocator>;

  mutable std::aligned_storage_t<sizeof(T), alignof(T)> storage_{};

  struct destroy
  {
    void operator()(T *p) const noexcept
    {
      if (p)
        p->~T();
    }
  };

  unique_ptr<T, destroy> p_;

public:
  /// Delete the managed object.
  inline void delete_managed_object() noexcept
  {
    GPCL_ASSERT(base_type::use_count() == 0);
    p_.reset();
  }

  inline std::nullptr_t get_deleter() noexcept { return nullptr; }

  static inline type_info *get_deleter_type_info() noexcept { return nullptr; }

public:
  template <typename... Args>
  explicit inline ref_count_obj(ref_count_base::operation_func_t op_func,
                                Allocator a, Args &&...args)
      : base_type(op_func, a),
        p_(::new (reinterpret_cast<void *>(&storage_))
               T(std::forward<Args>(args)...))
  {
  }

  T *get_address() const noexcept { return p_.get(); }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_REF_COUNT_HPP
