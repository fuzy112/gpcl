//
// ref_count_base.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_REF_COUNT_BASE_HPP
#define GPCL_DETAIL_REF_COUNT_BASE_HPP

#include <gpcl/assert.hpp>
#include <gpcl/atomic.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/ref_count_operation.hpp>

namespace gpcl {
namespace detail {

class ref_count_base
{
public:
  using operation_func_t = void *(*)(ref_count_base *self,
                                     ref_count_operation) noexcept;

  // Increment use count.
  void get() noexcept
  {
    GPCL_ASSERT(use_count() > 0);

    use_count_.fetch_add(1, gpcl::memory_order::relaxed);
  }

  // Decrement use count and if use count reaches zero, destroy the managed
  // object. After the managed object is destroyed, the weak count will be
  // decremented. If weak count reaches zero, the control block will be
  // destroyed.
  void put() noexcept
  {
    GPCL_ASSERT(use_count() > 0);

    if (use_count_.fetch_sub(1, gpcl::memory_order::acquire) == 1)
    {
      operate(ref_count_operation::destroy_managed_object);

      if (weak_count_.fetch_sub(1, gpcl::memory_order::acquire) == 1)
      {
        operate(ref_count_operation::delete_control_block);
      }
    }
  }

  // Returns the use count.
  long use_count() const noexcept
  {
    return use_count_.load(gpcl::memory_order::relaxed);
  }

  // Increment weak count.
  void weak_get() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);

    weak_count_.fetch_add(1, gpcl::memory_order::relaxed);
  }

  // Decrement weak count.
  // If weak count reaches zero, the control block will be destroyed.
  void weak_put() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    if (weak_count_.fetch_sub(1, gpcl::memory_order::acquire) == 1)
    {
      GPCL_ASSERT(use_count() == 0);
      operate(ref_count_operation::delete_control_block);
    }
  }

  // Returns the weak count.
  long weak_count() const noexcept
  {
    return weak_count_.load(gpcl::memory_order::relaxed);
  }

  // Try increment the use count, fail if use count equals 0.
  GPCL_NODISCARD bool lock() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    long old_val = use_count();

    while (old_val > 0)
    {
      if (use_count_.compare_exchange_weak(old_val, old_val + 1,
                                           gpcl::memory_order::release))
        return true;
    }

    return false;
  }

  // Invoke an operation.
  void *operate(ref_count_operation op) noexcept { return op_func_(this, op); }

protected:
  // Constructor.
  explicit constexpr ref_count_base(operation_func_t op_func) noexcept
      : op_func_(op_func)
  {
    GPCL_ASSERT(op_func_);
  }

private:
  // Use function pointer to avoid overhead of virtual functions.
  operation_func_t op_func_;

  gpcl::atomic<long> use_count_{1};

  gpcl::atomic<long> weak_count_{1};
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_REF_COUNT_BASE_HPP
