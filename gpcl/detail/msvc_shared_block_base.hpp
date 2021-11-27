//
// msvc_shared_block_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_MSVC_SHARED_BLOCK_BASE_HPP
#define GPCL_DETAIL_MSVC_SHARED_BLOCK_BASE_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/shared_block_operation.hpp>

#include <winbase.h>
#include <winnt.h>

namespace gpcl {
namespace detail {

class msvc_shared_block_base
{
public:
  typedef volatile unsigned long count_t;

  typedef void (*operation_func_t)(msvc_shared_block_base *self,
                                   shared_block_operation_t) noexcept;

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
      op_func_(this, destroy_managed_object);

      if (InterlockedDecrement(&weak_count_) == 0)
      {
        op_func_(this, delete_control_block);
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
      op_func_(this, delete_control_block);
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
    if (InterlockedIncrementNoFence(&use_count_) == 1)
    {
      use_count_ = 0;
      return false;
    }

    return true;
  }

protected:
  explicit msvc_shared_block_base(operation_func_t op_func) : op_func_(op_func)
  {
  }

private:
  operation_func_t op_func_ = nullptr;

  count_t use_count_ = 1;

  count_t weak_count_ = 1;
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_MSVC_SHARED_BLOCK_BASE_HPP
