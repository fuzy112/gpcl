//
// msvc_ref_count_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_MSVC_REF_COUNT_BASE_HPP
#define GPCL_DETAIL_MSVC_REF_COUNT_BASE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/ref_count_operation.hpp>

#include <windows.h>

namespace gpcl {
namespace detail {

class msvc_ref_count_base
{
public:
  typedef volatile unsigned long count_t;

  typedef void *(*operation_func_t)(msvc_ref_count_base *self,
                                    ref_count_operation_t) noexcept;

  void get() noexcept
  {
    LONG prev_use_count = InterlockedIncrementNoFence(&use_count_) - 1;
    GPCL_ASSERT(prev_use_count > 0);
    [](auto...) {}(prev_use_count);
  }

  void put() noexcept
  {
    LONG prev_use_count = InterlockedDecrement(&use_count_) + 1;
    GPCL_ASSERT(prev_use_count > 0);
    if (prev_use_count == 1)
    {
      operate(destroy_managed_object);

      if (InterlockedDecrement(&weak_count_) == 0)
      {
        operate(delete_control_block);
      }
    }
  }

  long use_count() const noexcept
  {
    long val = use_count_;
    return val;
  }

  void weak_get() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    InterlockedIncrementNoFence(&weak_count_);
  }

  void weak_put() noexcept
  {
    if (InterlockedDecrement(&weak_count_) == 0)
    {
      GPCL_ASSERT(use_count() == 0);
      operate(delete_control_block);
    }
  }

  long weak_count() const noexcept
  {
    long val = weak_count_;
    return val;
  }

  bool lock() noexcept
  {
    GPCL_ASSERT(weak_count() > 0);
    long old_val = use_count_;

    while (old_val > 0)
    {
      if (InterlockedCompareExchange(&use_count_, old_val + 1, old_val))
        return true;
    }

    return false;
  }

  void *operate(ref_count_operation_t op) noexcept
  {
    return op_func_(this, op);
  }

protected:
  explicit msvc_ref_count_base(operation_func_t op_func) : op_func_(op_func) {}

private:
  operation_func_t op_func_ = nullptr;

  count_t use_count_ = 1;

  count_t weak_count_ = 1;
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_MSVC_REF_COUNT_BASE_HPP
