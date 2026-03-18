//
// futex_atomic_wait.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_FUTEX_ATOMIC_WAIT_HPP
#define GPCL_DETAIL_FUTEX_ATOMIC_WAIT_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/futex.hpp>
#include <gpcl/detail/posix_thread.hpp>
#include <gpcl/memory_order.hpp>

namespace gpcl {
namespace detail {

using futex_word_type = i32;

struct futex_atomic_wait_state
{
  static constexpr auto alignment = 64;

  alignas(alignment) futex_word_type waiter_count = 0;

  alignas(alignment) futex_word_type proxy = 0;

  static futex_atomic_wait_state &for_(const void *addr) noexcept
  {
    static futex_atomic_wait_state arr[16];
    return arr[(std::uintptr_t(addr) >> 2) % 16];
  }

  void start_waiting() noexcept
  {
    __atomic_fetch_add(&waiter_count, 1, __ATOMIC_ACQ_REL);
  }

  void finish_waiting() noexcept
  {
    __atomic_fetch_sub(&waiter_count, 1, __ATOMIC_ACQ_REL);
  }

  bool is_waiting() const noexcept
  {
    futex_word_type n;
    __atomic_load(&waiter_count, &n, __ATOMIC_RELAXED);
    return n > 0;
  }

  struct waiter
  {
    futex_atomic_wait_state &state_;

    explicit waiter(futex_atomic_wait_state &state) noexcept : state_(state)
    {
      state_.start_waiting();
    }

    ~waiter() noexcept { state_.finish_waiting(); }
  };

  template <typename T>
  static futex_word_type *wait_address(const T *addr, futex_word_type *proxy)
  {
    (void)addr;
    GPCL_CXX17_IF_CONSTEXPR (sizeof(T) == sizeof(futex_word_type))
      return reinterpret_cast<futex_word_type *>(const_cast<T *>(addr));
    return proxy;
  }

  template <typename T>
  T wait(const T *addr, const T old, int order)
  {

    T val;
    futex_word_type event;
    futex_word_type *wait_addr = wait_address(addr, &proxy);

    __atomic_load(wait_addr, &event, __ATOMIC_ACQUIRE);

    __atomic_load(addr, &val, order);
    if (0 != memcmp(&val, &old, sizeof(old)))
      return val;

    waiter w(*this);

    do
    {
      futex_wait(wait_addr, event);
      __atomic_load(addr, &val, order);
    } while (!memcmp(&val, &old, sizeof(old)));
    return val;
  }

  template <typename T>
  void wake(T *addr, bool all)
  {
    futex_word_type *wait_addr = wait_address(addr, &proxy);
    GPCL_CXX17_IF_CONSTEXPR (sizeof(futex_word_type) != sizeof(T))
      __atomic_fetch_add(wait_addr, 1, __ATOMIC_ACQ_REL);
    else
      all = true;

    if (!is_waiting())
      return;
    i32 nwaiters;
    if (all)
      nwaiters = INT_MAX;
    else
      nwaiters = 1;
    futex_wake(wait_addr, nwaiters);
  }
};

template <typename T>
T futex_atomic_wait_on_address(const T *addr, T old, int order)
{
  return futex_atomic_wait_state::for_(addr).wait(addr, old, order);
}

template <typename T>
void futex_atomic_wake_by_address(T *addr, bool all)
{
  futex_atomic_wait_state::for_(addr).wake(addr, all);
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_FUTEX_ATOMIC_WAIT_HPP
