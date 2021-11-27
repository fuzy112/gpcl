//
// shared_block.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_SHARED_BLOCK_HPP
#define GPCL_DETAIL_SHARED_BLOCK_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/shared_block_base.hpp>

#include <memory>
#include <utility>

namespace gpcl {
namespace detail {

// shared_block has two forms of partial specializations:
// a. shared_block<Manager>
// b. shared_block<Manager, Allocator>
// where Manager is a specialization of either gpcl::optional or
// gpcl::unique_ptr.
//
// The first form of shared_block is created through new expression.
// The second form is created using allocator.
//
// While neither delete_this() nor delete_managed_object() function is public,
// the destruction of the managed object and the shared_block itself is
// performed by invoking shared_block_base's operate function.
//
// Instances of shared_block are meant to be created by calling the
// shared_block::create() member function.
template <typename... ManagerAndAllocator>
class shared_block;

template <typename Manager>
class shared_block<Manager> : public shared_block_base
{
private:
  Manager manager_;

  // Delete the shared_block itself.
  void delete_this() noexcept
  {
    GPCL_ASSERT(use_count() == 0);
    GPCL_ASSERT(weak_count() == 0);
    delete this;
  }

  // Delete the managed object.
  void delete_managed_object() noexcept
  {
    GPCL_ASSERT(use_count() == 0);
    manager_.reset();
  }

protected:
  // operation function.
  static void do_operate(shared_block_base *self,
                         shared_block_operation_t op) noexcept
  {
    auto s = static_cast<shared_block *>(self);
    switch (op)
    {
    case delete_control_block:
      s->delete_this();
      break;

    case destroy_managed_object:
      s->delete_managed_object();
      break;

    default:
      GPCL_UNREACHABLE("invalid shared_block operation");
    }
  }

  // @param op_func the operation function, normally &shared_block::do_operate.
  // @param args... arguments passed to the constructor of Manager.
  template <typename... Args>
  explicit shared_block(operation_func_t op_func, Args &&...args)
      : shared_block_base{op_func},
        manager_(std::forward<Args>(args)...)
  {
  }

public:
  // Access the managed object.
  decltype(std::addressof(*std::declval<Manager &>())) managed_object() noexcept
  {
    // Cannot call .get() or .value() simply.
    if (manager_)
      return std::addressof(*manager_);
    return nullptr;
  }

  // Create a shared_block.
  // @param args... arguments passed to constructor of Manager.
  template <typename... Args>
  static shared_block *create(Args &&...args)
  {
    return new shared_block(&do_operate, std::forward<Args>(args)...);
  }
};

// The Allocator has to be stored for deallocation and destruction of the
// shared_block itself. The Allocator is inherited to perform empty base class
// optimization.
template <typename Manager, typename Allocator>
class shared_block<Manager, Allocator> : public shared_block<Manager>,
                                         private Allocator
{
public:
  using rebind_allocator = typename std::allocator_traits<
      Allocator>::template rebind_alloc<shared_block>;
  using rebind_allocator_traits = std::allocator_traits<rebind_allocator>;

  // @param allocator allocator to allocate memory for the created shared_block.
  // @param args... arguments passed to the constructor of Manager.
  template <typename... Args>
  static shared_block *create(const Allocator &allocator, Args &&...args)
  {
    rebind_allocator alloc{allocator};
    shared_block *p{rebind_allocator_traits::allocate(alloc, 1)};
    GPCL_TRY
    {
      return new (p)
          shared_block(&do_operate, allocator, std::forward<Args>(args)...);
    }
    GPCL_CATCH(...)
    {
      rebind_allocator_traits::deallocate(alloc, p, 1);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

private:
  // destroy the shared_block itself then deallocate the memory.
  void delete_this() noexcept
  {
    GPCL_ASSERT(this->use_count() == 0);
    GPCL_ASSERT(this->weak_count() == 0);
    rebind_allocator alloc{*this};
    this->~shared_block();
    rebind_allocator_traits::deallocate(alloc, this, 1);
  }

protected:
  // operation function.
  static void do_operate(shared_block_base *self,
                         shared_block_operation_t op) noexcept
  {
    auto s = static_cast<shared_block *>(self);
    switch (op)
    {
    case delete_control_block:
      s->delete_this();
      break;

    default:
      shared_block<Manager>::do_operate(self, op);
      break;
    }
  }

  // @param op_func the operation function, normally &shared_block::do_operate.
  // @param alloc the allocator that allocates memory for the shared_block
  // instance.
  // @param args... arguments passed to the constructor of Manager.
  template <typename... Args>
  explicit shared_block(shared_block_base::operation_func_t op_func,
                        const Allocator &alloc, Args &&...args)
      : shared_block<Manager>{op_func, std::forward<Args>(args)...},
        Allocator{alloc}
  {
  }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_SHARED_BLOCK_HPP
