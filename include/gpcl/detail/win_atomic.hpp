//
// win_atomic.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_ATOMIC_HPP
#define GPCL_DETAIL_WIN_ATOMIC_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/atomic_facade.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/interlocked.hpp>
#include <gpcl/memory_order.hpp>

#define GPCL_ATOMIC_WARN_INVALID_MEMORDER(op)                                  \
  GPCL_UNREACHABLE("Invalid memory order for " #op " operation")

namespace gpcl {
namespace detail {

template <size_t Size>
struct win_atomic_ops;

#define GPCL_DEFINE_WIN_ATOMIC_OPS(bytes, bits, data_type)                     \
  template <>                                                                  \
  struct win_atomic_ops<bytes>                                                 \
  {                                                                            \
    static constexpr bool is_always_lock_free = true;                          \
                                                                               \
    template <typename T>                                                      \
    static bool is_lock_free(T const *ptr) noexcept                            \
    {                                                                          \
      return true;                                                             \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static void store(T *ptr, T *val, memory_order order) noexcept             \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        WriteNoFence##bits((data_type *)ptr, *(data_type *)val);               \
        break;                                                                 \
                                                                               \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(store);                              \
                                                                               \
      case memory_order::seq_cst:                                              \
      case memory_order::release:                                              \
        WriteRelease##bits((data_type *)ptr, *(data_type *)val);               \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static void load(T const *ptr, T *ret, memory_order order) noexcept        \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        *(data_type *)ret = ReadNoFence##bits((data_type *)ptr);               \
        break;                                                                 \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(load);                               \
                                                                               \
      case memory_order::acquire:                                              \
      case memory_order::seq_cst:                                              \
        *(data_type *)ret = ReadAcquire##bits((data_type *)ptr);               \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static void exchange(T *ptr, T *val, T *ret, memory_order order) noexcept  \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        *(data_type *)ret = InterlockedExchangeNoFence##bits(                  \
            (data_type *)ptr, *(data_type *)val);                              \
        break;                                                                 \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        *(data_type *)ret = InterlockedExchangeAcquire##bits(                  \
            (data_type *)ptr, *(data_type *)val);                              \
        break;                                                                 \
                                                                               \
      case memory_order::release:                                              \
        *(data_type *)ret = InterlockedExchangeRelease##bits(                  \
            (data_type *)ptr, *(data_type *)val);                              \
        break;                                                                 \
                                                                               \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(exchange);                           \
                                                                               \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        *(data_type *)ret =                                                    \
            InterlockedExchange##bits((data_type *)ptr, *(data_type *)val);    \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static bool compare_exchange(T *ptr, T *expected, T desired, bool weak,    \
                                 memory_order success, memory_order failure)   \
    {                                                                          \
      (void)failure;                                                           \
      (void)weak;                                                              \
      data_type initial;                                                       \
      switch (success)                                                         \
      {                                                                        \
      case memory_order::relaxed:                                              \
        initial = InterlockedCompareExchangeNoFence##bits(                     \
            (data_type *)ptr, *(data_type *)&desired, *(data_type *)expected); \
        break;                                                                 \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        initial = InterlockedCompareExchangeAcquire##bits(                     \
            (data_type *)ptr, *(data_type *)&desired, *(data_type *)expected); \
        break;                                                                 \
      case memory_order::release:                                              \
        initial = InterlockedCompareExchangeRelease##bits(                     \
            (data_type *)ptr, *(data_type *)&desired, *(data_type *)expected); \
        break;                                                                 \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(compare_exchange);                   \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        initial = InterlockedCompareExchange##bits(                            \
            (data_type *)ptr, *(data_type *)&desired, *(data_type *)expected); \
        break;                                                                 \
      }                                                                        \
      if (memcmp(&initial, expected, sizeof(*expected)) == 0)                  \
        return true;                                                           \
      memcpy(expected, &initial, sizeof(*expected));                           \
      return false;                                                            \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_add(T *ptr, data_type arg, memory_order order) noexcept     \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return (T)InterlockedExchangeAddNoFence##bits((data_type *)ptr, arg);  \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return (T)InterlockedExchangeAddAcquire##bits((data_type *)ptr, arg);  \
      case memory_order::release:                                              \
        return (T)InterlockedExchangeAddRelease##bits((data_type *)ptr, arg);  \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return (T)InterlockedExchangeAdd##bits((data_type *)ptr, arg);         \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_sub(T *ptr, data_type arg, memory_order order) noexcept     \
    {                                                                          \
      return fetch_add<T>(ptr, -static_cast<signed data_type>(arg), order);    \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_and(T *ptr, T arg, memory_order order) noexcept             \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedAndNoFence##bits((data_type *)ptr, arg);             \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedAndAcquire##bits((data_type *)ptr, arg);             \
      case memory_order::release:                                              \
        return InterlockedAndRelease##bits((data_type *)ptr, arg);             \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedAnd##bits((data_type *)ptr, arg);                    \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_or(T *ptr, T arg, memory_order order) noexcept              \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedOrNoFence##bits((data_type *)ptr, arg);              \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedOrAcquire##bits((data_type *)ptr, arg);              \
      case memory_order::release:                                              \
        return InterlockedOrRelease##bits((data_type *)ptr, arg);              \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedOr##bits((data_type *)ptr, arg);                     \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_xor(T *ptr, T arg, memory_order order) noexcept             \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedXorNoFence##bits((data_type *)ptr, arg);             \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedXorAcquire##bits((data_type *)ptr, arg);             \
      case memory_order::release:                                              \
        return InterlockedXorRelease##bits((data_type *)ptr, arg);             \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedXor##bits((data_type *)ptr, arg);                    \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static void wait(T *ptr, T *old, memory_order order) noexcept              \
    {                                                                          \
      T val;                                                                   \
      for (;;)                                                                 \
      {                                                                        \
        load(ptr, &val,                                                        \
             (memory_order)((int)order & (int)memory_order_acquire));          \
        if (memcmp(&val, old, sizeof(T)) != 0)                                 \
          return;                                                              \
        GPCL_VERIFY(WaitOnAddress(ptr, old, bytes, INFINITE));                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static void notify_one(T *ptr) noexcept                                    \
    {                                                                          \
      WakeByAddressSingle(ptr);                                                \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static void notify_all(T *ptr) noexcept                                    \
    {                                                                          \
      WakeByAddressAll(ptr);                                                   \
    }                                                                          \
  }

GPCL_DEFINE_WIN_ATOMIC_OPS(1, 8, char);
GPCL_DEFINE_WIN_ATOMIC_OPS(2, 16, short);
GPCL_DEFINE_WIN_ATOMIC_OPS(4, , long);
GPCL_DEFINE_WIN_ATOMIC_OPS(8, 64, __int64);

// todo: primary template for win_atomic_ops

// Generic functions

template <typename T>
using win_atomic_is_always_lock_free =
    std::bool_constant<win_atomic_ops<sizeof(T)>::is_always_lock_free>;

#define GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE(Type)                       \
  typename std::enable_if<                                                     \
      gpcl::detail::win_atomic_is_always_lock_free<Type>::value, int>::type =  \
      0

#define GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(Type)              \
  template <typename GPCLType = Type,                                          \
            GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE(GPCLType)>

template <typename T>
bool win_atomic_is_lock_free(T const *ptr) noexcept
{
  return win_atomic_ops<sizeof(T)>::is_lock_free(ptr);
}

template <typename T>
void win_atomic_store(T *ptr, T *val, memory_order order) noexcept
{
  win_atomic_ops<sizeof(T)>::store(ptr, val, order);
}

template <typename T>
void win_atomic_load(T const *ptr, T *ret, memory_order order) noexcept
{
  win_atomic_ops<sizeof(T)>::load(ptr, ret, order);
}

template <typename T>
void win_atomic_exchange(T *ptr, T *val, T *ret, memory_order order) noexcept
{
  win_atomic_ops<sizeof(T)>::exchange(ptr, val, ret, order);
}

template <typename T>
bool win_atomic_compare_exchange(T *ptr, T *expected, T desired, bool weak,
                                 memory_order success,
                                 memory_order failure) noexcept
{
  return win_atomic_ops<sizeof(T)>::compare_exchange(ptr, expected, desired,
                                                     weak, success, failure);
}

template <typename T, typename U>
T win_atomic_fetch_add(T *ptr, U arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_add(ptr, arg, order);
}

template <typename T, typename U>
T win_atomic_fetch_sub(T *ptr, U arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_sub(ptr, arg, order);
}

template <typename T>
T win_atomic_fetch_and(T *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_and(ptr, arg, order);
}

template <typename T>
T win_atomic_fetch_or(T *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_or(ptr, arg, order);
}

template <typename T>
T win_atomic_fetch_xor(T *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_xor(ptr, arg, order);
}

template <typename T>
void win_atomic_wait(T *ptr, T *old, memory_order order) noexcept
{
  win_atomic_ops<sizeof(T)>::wait(ptr, old, order);
}

template <typename T>
void win_atomic_notify_one(T *ptr) noexcept
{
  win_atomic_ops<sizeof(T)>::notify_one(ptr);
}

template <typename T>
void win_atomic_notify_all(T *ptr) noexcept
{
  win_atomic_ops<sizeof(T)>::notify_all(ptr);
}

template <typename T>
struct win_atomic_base
{
  typedef T value_type;

  static constexpr bool is_always_lock_free =
      win_atomic_is_always_lock_free<T>();

  T value;

  constexpr win_atomic_base() noexcept(std::is_nothrow_constructible<T>::value)
      : value()
  {
  }

  constexpr win_atomic_base(T desired) noexcept : value(desired) {}

  win_atomic_base(const win_atomic_base &) = delete;

  win_atomic_base &operator=(const win_atomic_base) = delete;

  bool is_lock_free() const volatile noexcept { return win_atomic_ops(&value); }

  T operator=(T desired) noexcept
  {
    store(desired);
    return desired;
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T operator=(T desired) volatile noexcept
  {
    store(desired);
    return desired;
  }

  operator T() const noexcept { return load(); }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  operator T() const volatile noexcept { return load(); }

  void store(T desired, memory_order order = memory_order::seq_cst) noexcept
  {
    win_atomic_store(&value, &desired, order);
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  void store(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    win_atomic_store(&value, &desired, order);
  }

  T load(memory_order order = memory_order::seq_cst) const noexcept
  {
    T ret;
    win_atomic_load(&value, &ret, order);
    return ret;
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T load(memory_order order = memory_order::seq_cst) const volatile noexcept
  {
    T ret;
    return win_atomic_load(&value, &ret, order);
    return ret;
  }

  T exchange(T desired, memory_order order = memory_order::seq_cst) noexcept
  {
    T ret;
    win_atomic_exchange(&value, &desired, &ret, order);
    return ret;
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T exchange(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    T ret;
    win_atomic_exchange(&value, &desired, &ret, order);
    return ret;
  }

  bool compare_exchange_strong(T &expected, T desired, memory_order success,
                               memory_order failure) noexcept
  {
    return win_atomic_compare_exchange(&value, &expected, desired, false,
                                       success, failure);
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_strong(T &expected, T desired, memory_order success,
                               memory_order failure) volatile noexcept
  {
    return win_atomic_compare_exchange(&value, &expected, desired, false,
                                       success, failure);
  }

  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order const order = memory_order::seq_cst) noexcept
  {
    memory_order failure{order};
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_strong(expected, desired, order, failure);
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order const order = memory_order::seq_cst) volatile noexcept
  {
    memory_order failure{order};
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_strong(expected, desired, order, failure);
  }

  bool compare_exchange_weak(T &expected, T desired, memory_order success,
                             memory_order failure) noexcept
  {
    return win_atomic_compare_exchange(&value, &expected, desired, true,
                                       success, failure);
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_weak(T &expected, T desired, memory_order success,
                             memory_order failure) volatile noexcept
  {
    return win_atomic_compare_exchange(&value, &expected, desired, true,
                                       success, failure);
  }

  bool compare_exchange_weak(
      T &expected, T desired,
      memory_order const order = memory_order::seq_cst) noexcept
  {
    memory_order failure{order};
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_weak(expected, desired, order, failure);
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  bool compare_exchange_weak(
      T &expected, T desired,
      memory_order const order = memory_order::seq_cst) volatile noexcept
  {
    memory_order failure{order};
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_weak(expected, desired, order, failure);
  }

  void wait(T old, memory_order order = memory_order::seq_cst) noexcept
  {
    win_atomic_wait(&value, &old, order);
  }

  void notify_one() noexcept { win_atomic_notify_one(&value); }

  void notify_all() noexcept { win_atomic_notify_all(&value); }
};

template <typename T>
struct win_atomic : win_atomic_base<T>, atomic_facade<win_atomic<T>, T, T>
{
  using win_atomic_base<T>::win_atomic_base;
  using win_atomic_base<T>::operator=;

  T fetch_add(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return win_atomic_fetch_add(&this->value, arg, (order));
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_add(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return win_atomic_fetch_add(&this->value, arg, (order));
  }

  T fetch_sub(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return win_atomic_fetch_sub(&this->value, arg, (order));
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_sub(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return win_atomic_fetch_sub(&this->value, arg, (order));
  }

  T fetch_and(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return win_atomic_fetch_and(&this->value, arg, (order));
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_and(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return win_atomic_fetch_and(&this->value, arg, (order));
  }

  T fetch_xor(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return win_atomic_fetch_xor(&this->value, arg, (order));
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_xor(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return win_atomic_fetch_xor(&this->value, arg, (order));
  }

  T fetch_or(T arg, memory_order order = memory_order::seq_cst) noexcept
  {
    return win_atomic_fetch_or(&this->value, arg, (order));
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T fetch_or(T arg,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return win_atomic_fetch_or(&this->value, arg, (order));
  }
};

template <typename T>
struct win_atomic<T *> : win_atomic_base<T *>,
                         atomic_facade<win_atomic<T *>, T *, std::ptrdiff_t>
{
  using win_atomic_base<T *>::win_atomic_base;
  using win_atomic_base<T *>::operator=;

  T *fetch_add(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) noexcept
  {
    return win_atomic_fetch_add(&this->value, sizeof(T) * arg, (order));
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T *fetch_add(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return win_atomic_fetch_add(&this->value, sizeof(T) * arg, (order));
  }

  T *fetch_sub(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) noexcept
  {
    return win_atomic_fetch_sub(&this->value, sizeof(T) * arg, (order));
  }

  GPCL_WIN_ATOMIC_ENABLE_IF_ALWAYS_LOCK_FREE_TEMPLATE(T)
  T *fetch_sub(std::ptrdiff_t arg,
               memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return win_atomic_fetch_sub(&this->value, sizeof(T) * arg, (order));
  }
};

} // namespace detail
} // namespace gpcl

#endif
