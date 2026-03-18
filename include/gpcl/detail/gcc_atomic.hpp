//
// gcc_atomic.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_GCC_ATOMIC_HPP
#define GPCL_DETAIL_GCC_ATOMIC_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/detail/atomic_facade.hpp>
#include <gpcl/detail/assert.hpp>

#ifdef GPCL_LINUX
#  include <gpcl/detail/futex_atomic_wait.hpp>
#elif defined(GPCL_WINDOWS)
#  include <windows.h>
#endif

namespace gpcl {
namespace detail {

inline int gcc_memory_order(memory_order order) noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return __ATOMIC_RELAXED;
  case memory_order::consume:
    return __ATOMIC_CONSUME;
  case memory_order::acquire:
    return __ATOMIC_ACQUIRE;
  case memory_order::release:
    return __ATOMIC_RELEASE;
  case memory_order::acq_rel:
    return __ATOMIC_ACQ_REL;
  case memory_order::seq_cst:
    return __ATOMIC_SEQ_CST;
  default:
    GPCL_UNREACHABLE("Invalid memory order");
  }
}

#define GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(Type)              \
  template <typename GpclDummyType = int,                                      \
            typename std::enable_if<                                           \
                gpcl::detail::gcc_atomic_base<Type>::is_always_lock_free,      \
                GpclDummyType>::type = 0>

template <typename T>
struct gcc_atomic_base
{
  typedef T value_type;

  static_assert(std::is_trivially_copyable<T>::value, "");

  static constexpr bool is_always_lock_free =
      __atomic_always_lock_free(sizeof(T), 0);

  bool is_lock_free() const volatile noexcept
  {
    return __atomic_is_lock_free(sizeof(T), &value);
  }

  T value;

  constexpr gcc_atomic_base() noexcept(
      std::is_nothrow_default_constructible<T>::value)
      : value()
  {
  }
  constexpr gcc_atomic_base(T desired) : value(desired) {}

  T operator=(T desired) noexcept
  {
    store(desired);
    return desired;
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T operator=(T desired) volatile noexcept
  {
    store(desired);
    return desired;
  }

  operator T() const noexcept { return load(); }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  operator T() const volatile noexcept { return load(); }

  void wait(T old, memory_order order = memory_order::seq_cst) volatile noexcept
  {
#if defined GPCL_LINUX
    futex_atomic_wait_on_address((T *)&value, old, gcc_memory_order(order));
#elif defined GPCL_WINDOWS
    WaitOnAddress((T *)&value, &old, sizeof(old), INFINITE);
#endif
  }

  void notify_all() volatile noexcept
  {
#if defined GPCL_LINUX
    futex_atomic_wake_by_address((T *)&value, true);
#elif defined GPCL_WINDOWS
    WakeByAddressAll((T *)&value);
#endif
  }

  void notify_one() volatile noexcept
  {
#if defined GPCL_LINUX
    futex_atomic_wake_by_address((T *)&value, false);
#elif defined GPCL_WINDOWS
    WakeByAddressSingle((T *)&value);
#endif
  }

  void store(T desired, memory_order order = memory_order::seq_cst) noexcept
  {
    __atomic_store(&value, &desired, gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  void store(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    __atomic_store(&value, &desired, gcc_memory_order(order));
  }

  T load(memory_order order = memory_order::seq_cst) const noexcept
  {
    T result;
    __atomic_load(&value, &result, gcc_memory_order(order));
    return result;
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T load(memory_order order = memory_order::seq_cst) const volatile noexcept
  {
    T result;
    __atomic_load(&value, &result, gcc_memory_order(order));
    return result;
  }

  T exchange(T desired, memory_order order = memory_order::seq_cst) noexcept
  {
    T result;
    __atomic_exchange(&value, &desired, &result, gcc_memory_order(order));
    return result;
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T exchange(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    T result;
    __atomic_exchange(&value, &desired, &result, gcc_memory_order(order));
    return result;
  }

  bool compare_exchange_strong(T &expected, T desired, memory_order success,
                               memory_order failure) noexcept
  {
    return __atomic_compare_exchange(&value, &expected, &desired, false,
                                     gcc_memory_order(success),
                                     gcc_memory_order(failure));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_strong(T &expected, T desired, memory_order success,
                               memory_order failure) volatile noexcept
  {
    return __atomic_compare_exchange(&value, &expected, &desired, false,
                                     gcc_memory_order(success),
                                     gcc_memory_order(failure));
  }

  bool
  compare_exchange_strong(T &expected, T desired,
                          memory_order order = memory_order::seq_cst) noexcept
  {
    memory_order failure = order;
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_strong(expected, desired, order, failure);
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept
  {
    memory_order failure = order;
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_strong(expected, desired, order, failure);
  }

  bool compare_exchange_weak(T &expected, T desired, memory_order success,
                             memory_order failure) noexcept
  {
    return __atomic_compare_exchange(&value, &expected, &desired, true,
                                     gcc_memory_order(success),
                                     gcc_memory_order(failure));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_weak(T &expected, T desired, memory_order success,
                             memory_order failure) volatile noexcept
  {
    return __atomic_compare_exchange(&value, &expected, &desired, true,
                                     gcc_memory_order(success),
                                     gcc_memory_order(failure));
  }

  bool
  compare_exchange_weak(T &expected, T desired,
                        memory_order order = memory_order::seq_cst) noexcept
  {
    memory_order failure = order;
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_weak(expected, desired, order, failure);
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_weak(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept
  {
    memory_order failure = order;
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_weak(expected, desired, order, failure);
  }
};

template <typename T>
struct gcc_atomic : gcc_atomic_base<T>, atomic_facade<gcc_atomic<T>, T>
{
  using gcc_atomic_base<T>::gcc_atomic_base;

  T fetch_add(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return __atomic_fetch_add(&this->value, arg, gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_add(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_add(&this->value, arg, gcc_memory_order(order));
  }

  T fetch_sub(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return __atomic_fetch_sub(&this->value, arg, gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_sub(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_sub(&this->value, arg, gcc_memory_order(order));
  }

  T fetch_and(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return __atomic_fetch_and(&this->value, arg, gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_and(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_and(&this->value, arg, gcc_memory_order(order));
  }

  T fetch_xor(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return __atomic_fetch_xor(&this->value, arg, gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_xor(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_xor(&this->value, arg, gcc_memory_order(order));
  }

  T fetch_or(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return __atomic_fetch_or(&this->value, arg, gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_or(T arg,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_or(&this->value, arg, gcc_memory_order(order));
  }
};

template <typename T>
struct gcc_atomic<T *> : gcc_atomic_base<T *>,
                         atomic_facade<gcc_atomic<T *>, T *, std::ptrdiff_t>
{
  using gcc_atomic_base<T *>::gcc_atomic_base;
  using gcc_atomic_base<T *>::operator=;

  T *fetch_add(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) noexcept
  {
    return __atomic_fetch_add(&this->value, sizeof(T) * arg,
                              gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T *fetch_add(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_add(&this->value, sizeof(T) * arg,
                              gcc_memory_order(order));
  }

  T *fetch_sub(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) noexcept
  {
    return __atomic_fetch_sub(&this->value, sizeof(T) * arg,
                              gcc_memory_order(order));
  }

  GPCL_GCC_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T *fetch_sub(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_sub(&this->value, sizeof(T) * arg,
                              gcc_memory_order(order));
  }
};

} // namespace detail
} // namespace gpcl

#endif //
