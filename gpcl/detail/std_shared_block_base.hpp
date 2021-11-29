//
// std_shared_block_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_STD_SHARED_BLOCK_BASE_HPP
#define GPCL_DETAIL_STD_SHARED_BLOCK_BASE_HPP

#  include <gpcl/assert.hpp>
#  include <gpcl/detail/config.hpp>
#  include <gpcl/detail/shared_block_operation.hpp>

#include <atomic>

namespace gpcl {
namespace detail {

class std_shared_block_base
{
public:
  typedef void (*operation_func_t)(std_shared_block_base *self,
                                   shared_block_operation_t) noexcept;

  // Increment use count.
  void get() noexcept
  {
    GPCL_ASSERT(use_count() > 0);

    use_count_.fetch_add(1, std::memory_order_relaxed);
  }

  // Decrement use count and if use count reaches zero, destroy the managed
  // object. After the managed object is destroyed, the weak count will be
  // decremented. If weak count reaches zero, the control block will be
  // destroyed.
  void put() noexcept
  {
    GPCL_ASSERT(use_count() > 0);

    if (use_count_.fetch_sub(1, std::memory_order_acquire) == 1)
    {
      operate(destroy_managed_object);

      if (weak_count_.fetch_sub(1, std::memory_order_acquire) == 1)
      {
        operate(delete_control_block);
      }
    }
  }

  // Returns the use count.
  long use_count() const noexcept
  {
    return use_count_.load(std::memory_order_relaxed);
  }

  // Increment weak count.
  void weak_get() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    
    weak_count_.fetch_add(1, std::memory_order_relaxed);
  }

  // Decrement weak count.
  // If weak count reaches zero, the control block will be destroyed.
  void weak_put() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    if (weak_count_.fetch_sub(1, std::memory_order_acquire) == 1)
    {
      GPCL_ASSERT(use_count() == 0);
      operate(delete_control_block);
    }
  }

  // Returns the weak count.
  long weak_count() const noexcept
  {
    return weak_count_.load(std::memory_order_relaxed);
  }

  // Try increment the use count, fail if use count equals 0.
  bool lock() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    long old_val = use_count();

    while (old_val > 0)
    {
      if (use_count_.compare_exchange_weak(old_val, old_val + 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed))
        return true;
    }

    return false;
  }

  // Invoke an operation.
  void operate(shared_block_operation_t op) noexcept { op_func_(this, op); }

protected:
  // Constructor.
  explicit std_shared_block_base(operation_func_t op_func) : op_func_(op_func)
  {
    GPCL_ASSERT(op_func_);
  }

private:
  // Use function pointer to avoid overhead of virtual functions.
  operation_func_t op_func_;

  std::atomic<long> use_count_ = 1;

  std::atomic<long> weak_count_ = 1;
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_STD_SHARED_BLOCK_BASE_HPP
