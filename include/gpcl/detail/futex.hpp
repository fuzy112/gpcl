//
// futex.hpp
// ~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_FUTEX_HPP
#define GPCL_DETAIL_FUTEX_HPP

#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/inttypes.hpp>

#include <climits>
#include <linux/futex.h>
#include <sys/time.h>

namespace gpcl {
namespace detail {

typedef i32 futex_word_type;

GPCL_DECL int futex(futex_word_type *uaddr, int futex_op, futex_word_type val,
                    const struct timespec *timeout, futex_word_type *uaddr2,
                    futex_word_type val3) noexcept;

GPCL_DECL int futex(futex_word_type *uaddr, int futex_op, futex_word_type val,
                    u32 val2, futex_word_type *uaddr2,
                    futex_word_type val3) noexcept;

inline void futex_up(futex_word_type *uaddr, i32 nwaiters)
{
  //  To "up" a futex, execute the proper assembler instructions that
  //   will cause the host CPU to atomically increment the integer.
  futex_word_type old = __atomic_fetch_add(uaddr, 1, __ATOMIC_ACQ_REL);

  //  Afterward, check if it has in fact changed from 0 to 1, in which
  //   case there were no waiters and the operation is done.  This is
  //  the noncontended case which is fast and should be common.
  if (old == 0)
    return;

  // In the contended case, the atomic increment changed the counter
  //  from -1  (or some other negative number).  If this is detected,
  //  there are waiters.  User space should now set the counter to 1
  //  and instruct the kernel to wake up any waiters using the
  //  FUTEX_WAKE operation.
  futex_word_type val = 1;
  __atomic_store(uaddr, &val, __ATOMIC_RELAXED);
  int s = futex(uaddr, FUTEX_WAKE, nwaiters, nullptr, nullptr, 0);
  if (s == -1)
    GPCL_UNREACHABLE("futex-UP");
}

inline void futex_down(futex_word_type *uaddr)
{
  // Atomically decrement the counter and check if it changed to 0, in
  // which case the operation is done and the futex was uncontended.
  if (__atomic_sub_fetch(uaddr, 1, __ATOMIC_ACQ_REL) == 0)
    return;

  // In all other circumstances, the process should set the counter to
  //  -1 and request that the kernel wait for another process to up the
  // futex.  This is done using the FUTEX_WAIT operation.
  futex_word_type val = -1;
  __atomic_store(uaddr, &val, __ATOMIC_RELAXED);

  int s = futex(uaddr, FUTEX_WAIT, val, nullptr, nullptr, 0);
  if (s == 0)
    return;
  if (errno == EAGAIN || errno == ETIMEDOUT)
    return;
  GPCL_UNREACHABLE("futex-WAIT");
}

inline void futex_wake(i32 *uaddr, i32 nwaiters) noexcept
{
  int s = futex(uaddr, FUTEX_WAKE, nwaiters, nullptr, nullptr, 0);
  if (s == -1)
    GPCL_UNREACHABLE("futex-WAKE");
}

inline void futex_wake_one(i32 *uaddr) noexcept
{
  return futex_wake(uaddr, 1);
}

inline void futex_wake_all(i32 *uaddr) noexcept
{
  return futex_wake(uaddr, INT_MAX);
}

inline bool futex_wait(i32 *uaddr, i32 old, clockid_t clock, bool absolute,
                       const struct timespec *timeout) noexcept
{
  i32 futex_op = 0;
  i32 val3 = 0;

  switch (clock)
  {
  case CLOCK_MONOTONIC:
    break;
  case CLOCK_REALTIME:
    futex_op |= FUTEX_CLOCK_REALTIME;
    break;
  default:
    GPCL_UNREACHABLE("only CLOCK_MONOTONIC and CLOCK_REALTIME are supported");
  }
  if (absolute)
  {
    futex_op |= FUTEX_WAIT_BITSET;
    val3 = FUTEX_BITSET_MATCH_ANY;
  }
  else
  {
    futex_op |= FUTEX_WAIT;
  }

  int s = futex(uaddr, futex_op, old, timeout, nullptr, val3);
  if (s == 0)
    return true;
  if (errno == EAGAIN || errno == ETIMEDOUT)
    return false;
  GPCL_UNREACHABLE("futex-WAIT");
}

inline bool futex_wait(i32 *uaddr, i32 old)
{
  return futex_wait(uaddr, old, CLOCK_MONOTONIC, false, nullptr);
}

template <typename Period, typename Rep>
inline bool futex_wait_for(i32 *uaddr, i32 old,
                           chrono::duration<Period, Rep> timeout);

template <typename Duration>
inline bool futex_wait_until(i32 *uaddr, i32 old,
                             chrono::time_point<chrono::system_clock> timeout);

template <typename Duration>
inline bool futex_wait_until(i32 *uaddr, i32 old,
                             chrono::time_point<chrono::steady_clock> timeout);

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_FUTEX_HPP
