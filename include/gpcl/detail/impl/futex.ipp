//
// futex.ipp
// ~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_FUTEX_IPP
#define GPCL_DETAIL_IMPL_FUTEX_IPP

#include <gpcl/detail/futex.hpp>
#include <sys/syscall.h>

#include <unistd.h>

namespace gpcl {
namespace detail {

int futex(i32 *uaddr, int futex_op, i32 val, const struct timespec *timeout,
          i32 *uaddr2, i32 val3) noexcept
{
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

int futex(i32 *uaddr, int futex_op, i32 val, u32 val2, i32 *uaddr2,
          i32 val3) noexcept
{
  return syscall(SYS_futex, uaddr, futex_op, val, val2, uaddr2, val3);
}

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_FUTEX_IPP
