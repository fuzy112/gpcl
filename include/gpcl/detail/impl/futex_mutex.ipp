//
// futex_mutex.ipp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_FUTEX_MUTEX_IPP
#define GPCL_DETAIL_IMPL_FUTEX_MUTEX_IPP

#include <gpcl/detail/futex.hpp>
#include <gpcl/detail/futex_mutex.hpp>

namespace gpcl {
namespace detail {

void futex_mutex::lock() noexcept
{
  const int spin_count = 10;

  for (int i = 0; i < spin_count; ++i)
  {
    futex_word_type unlocked = futex_unlocked;

    if (__atomic_compare_exchange_n(&fut_, &unlocked, futex_locked, false,
                                    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
      return;
  }

  for (;;)
  {
    futex_word_type unlocked = futex_unlocked;

    if (__atomic_compare_exchange_n(&fut_, &unlocked, futex_locked, false,
                                    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
      return;
    int s = futex(&fut_, FUTEX_WAIT, 0, nullptr, nullptr, 0);
    if (s == -1 && errno != EAGAIN)
    {
      GPCL_UNREACHABLE("futex-FUTEX_WAIT");
    }
  }
}

void futex_mutex::unlock() noexcept
{
  futex_word_type locked = futex_locked;

  if (__atomic_compare_exchange_n(&fut_, &locked, futex_unlocked, false,
                                  __ATOMIC_RELEASE, __ATOMIC_RELAXED))
  {
    int s = futex(&fut_, FUTEX_WAKE, 1, nullptr, nullptr, 0);
    if (s == -1)
    {
      GPCL_UNREACHABLE("futex-FUTEX_WAKE");
    }
    return;
  }

  GPCL_UNREACHABLE("futex not locked");
}

bool futex_mutex::try_lock() noexcept
{
  futex_word_type unlocked = futex_unlocked;
  return __atomic_compare_exchange_n(&fut_, &unlocked, futex_locked, false,
                                     __ATOMIC_RELEASE, __ATOMIC_RELAXED);
}

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_FUTEX_MUTEX_IPP
