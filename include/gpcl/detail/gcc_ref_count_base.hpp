//
// gcc_ref_count_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_GCC_REF_COUNT_BASE_HPP
#define GPCL_DETAIL_GCC_REF_COUNT_BASE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/ref_count_operation.hpp>
#include <gpcl/noncopyable.hpp>

namespace gpcl {
namespace detail {

class gcc_ref_count_base : noncopyable
{
public:
  typedef long count_t;

  typedef void *(*operation_func_t)(gcc_ref_count_base *self,
                                    ref_count_operation) noexcept;

  // Increment use count.
  void get() noexcept
  {
    GPCL_ASSERT(use_count() > 0);
    __atomic_fetch_add(&use_count_, 1, __ATOMIC_RELAXED);
  }

  // Decrement use count and if use count reaches zero, destroy the managed
  // object. After the managed object is destroyed, the weak count will be
  // decremented. If weak count reaches zero, the control block will be
  // destroyed.
  void put() noexcept
  {
    GPCL_ASSERT(use_count() > 0);

    if (__atomic_sub_fetch(&use_count_, 1, __ATOMIC_ACQ_REL) == 0)
    {
      operate(ref_count_operation::destroy_managed_object);

      if (__atomic_sub_fetch(&weak_count_, 1, __ATOMIC_RELAXED) == 0)
      {
        operate(ref_count_operation::delete_control_block);
      }
    }
  }

  // Returns the use count.
  long use_count() const noexcept
  {
    long val;
    __atomic_load(&use_count_, &val, __ATOMIC_RELAXED);
    return val;
  }

  // Increment weak count.
  void weak_get() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    __atomic_fetch_add(&weak_count_, 1, __ATOMIC_RELAXED);
  }

  // Decrement weak count.
  // If weak count reaches zero, the control block will be destroyed.
  void weak_put() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    if (__atomic_sub_fetch(&weak_count_, 1, __ATOMIC_ACQ_REL) == 0)
    {
      GPCL_ASSERT(use_count() == 0);
      operate(ref_count_operation::delete_control_block);
    }
  }

  // Returns the weak count.
  long weak_count() const noexcept
  {
    long val;
    __atomic_load(&weak_count_, &val, __ATOMIC_SEQ_CST);
    return val;
  }

  // Try increment the use count, fail if use count equals 0.
  bool lock() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    long old_val = use_count();

    while (old_val > 0)
    {
      long new_val = old_val + 1;
      if (__atomic_compare_exchange(&use_count_, &old_val, &new_val, true,
                                    __ATOMIC_RELEASE, __ATOMIC_RELAXED))
        return true;
    }

    return false;
  }

  // Invoke an operation.
  void *operate(ref_count_operation op) noexcept
  {
    return op_func_(this, op);
  }

protected:
  // Constructor.
  explicit gcc_ref_count_base(operation_func_t op_func) : op_func_(op_func)
  {
    GPCL_ASSERT(op_func_);
  }

private:
  // Use function pointer to avoid overhead of virtual functions.
  operation_func_t op_func_;

  count_t use_count_ = 1;

  count_t weak_count_ = 1;
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_GCC_REF_COUNT_BASE_HPP
