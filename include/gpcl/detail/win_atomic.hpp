#ifndef GPCL_DETAIL_WIN_ATOMIC_HPP
#define GPCL_DETAIL_WIN_ATOMIC_HPP

#include <gpcl/detail/assert.hpp>
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
    static bool is_lock_free(T const volatile *ptr) { return true; }           \
                                                                               \
    template <typename T>                                                      \
    static void store(T volatile *ptr, T val, memory_order order) noexcept     \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        WriteNoFence##bits((data_type volatile *)ptr, val);                    \
        break;                                                                 \
                                                                               \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(store);                              \
                                                                               \
      case memory_order::seq_cst:                                              \
      case memory_order::release:                                              \
        WriteRelease##bits((data_type volatile *)ptr, val);                    \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T load(T const volatile *ptr, memory_order order) noexcept          \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return ReadNoFence##bits((data_type const volatile *)ptr);             \
                                                                               \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(load);                               \
                                                                               \
      case memory_order::acquire:                                              \
      case memory_order::seq_cst:                                              \
        return ReadAcquire##bits((data_type const volatile *)load);            \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T exchange(T volatile *ptr, T val, memory_order order) noexcept     \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedExchangeNoFence##bits((data_type volatile *)ptr,     \
                                                val);                          \
                                                                               \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedExchangeAcquire##bits((data_type volatile *)ptr,     \
                                                val);                          \
                                                                               \
      case memory_order::release:                                              \
        return InterlockedExchangeRelease##bits((data_type volatile *)ptr,     \
                                                val);                          \
                                                                               \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(exchange);                           \
                                                                               \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedExchange##bits((data_type volatile *)ptr, val);      \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static bool compare_exchange_strong(T volatile *ptr, T &expected,          \
                                        T desired,                             \
                                        memory_order order) noexcept           \
    {                                                                          \
      const T comparand = expected;                                            \
                                                                               \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        expected = InterlockedCompareExchangeNoFence##bits(                    \
            (data_type volatile *)ptr, desired, expected);                     \
        break;                                                                 \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        expected = InterlockedCompareExchangeAcquire##bits(                    \
            (data_type volatile *)ptr, desired, expected);                     \
        break;                                                                 \
      case memory_order::release:                                              \
        expected = InterlockedCompareExchangeRelease##bits(                    \
            (data_type volatile *)ptr, desired, expected);                     \
        break;                                                                 \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(compare_exchange_strong);            \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        expected = InterlockedCompareExchange##bits((data_type volatile *)ptr, \
                                                    desired, expected);        \
        break;                                                                 \
      }                                                                        \
                                                                               \
      return comparand == expected;                                            \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static bool compare_exchange_strong(T volatile *ptr, T &expected,          \
                                        T desired, memory_order success,       \
                                        memory_order failure) noexcept         \
    {                                                                          \
      if (failure == memory_order::acquire ||                                  \
          failure == memory_order::consume)                                    \
      {                                                                        \
        if (success == memory_order::relaxed)                                  \
          success = failure;                                                   \
        else if (success == memory_order::release)                             \
          success = memory_order::acq_rel;                                     \
        else if (success == memory_order::consume &&                           \
                 failure == memory_order::acquire)                             \
          success = memory_order::acquire;                                     \
      }                                                                        \
      return compare_exchange_strong(ptr, expected, desired, success);         \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static bool compare_exchange_weak(T volatile *ptr, T &expected, T desired, \
                                      memory_order order) noexcept             \
    {                                                                          \
      return compare_exchange_strong(ptr, expected, desired, order);           \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static bool compare_exchange_weak(T volatile *ptr, T &expected, T desired, \
                                      memory_order success,                    \
                                      memory_order failure) noexcept           \
    {                                                                          \
      if (failure == memory_order::acquire ||                                  \
          failure == memory_order::consume)                                    \
      {                                                                        \
        if (success == memory_order::relaxed)                                  \
          success = failure;                                                   \
        else if (success == memory_order::release)                             \
          success = memory_order::acq_rel;                                     \
        else if (success == memory_order::consume &&                           \
                 failure == memory_order::acquire)                             \
          success = memory_order::acquire;                                     \
      }                                                                        \
      return compare_exchange_weak(ptr, expected, desired, success);           \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_add(T volatile *ptr, T arg, memory_order order) noexcept    \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedExchangeAddNoFence##bits((data_type volatile *)ptr,  \
                                                   arg);                       \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedExchangeAddAcquire##bits((data_type volatile *)ptr,  \
                                                   arg);                       \
      case memory_order::release:                                              \
        return InterlockedExchangeAddRelease##bits((data_type volatile *)ptr,  \
                                                   arg);                       \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedExchangeAdd##bits((data_type volatile *)ptr, arg);   \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_sub(T volatile *ptr, T arg, memory_order order) noexcept    \
    {                                                                          \
      return fetch_add(ptr, -static_cast<signed data_type>(arg), order);       \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_and(T volatile *ptr, T arg, memory_order order) noexcept    \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedAndNoFence##bits((data_type volatile *)ptr, arg);    \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedAndAcquire##bits((data_type volatile *)ptr, arg);    \
      case memory_order::release:                                              \
        return InterlockedAndRelease##bits((data_type volatile *)ptr, arg);    \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedAnd##bits((data_type volatile *)ptr, arg);           \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_or(T volatile *ptr, T arg, memory_order order) noexcept     \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedOrNoFence##bits((data_type volatile *)ptr, arg);     \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedOrAcquire##bits((data_type volatile *)ptr, arg);     \
      case memory_order::release:                                              \
        return InterlockedOrRelease##bits((data_type volatile *)ptr, arg);     \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedOr##bits((data_type volatile *)ptr, arg);            \
      }                                                                        \
    }                                                                          \
                                                                               \
    template <typename T>                                                      \
    static T fetch_xor(T volatile *ptr, T arg, memory_order order) noexcept    \
    {                                                                          \
      switch (order)                                                           \
      {                                                                        \
      case memory_order::relaxed:                                              \
        return InterlockedXorNoFence##bits((data_type volatile *)ptr, arg);    \
      case memory_order::consume:                                              \
      case memory_order::acquire:                                              \
        return InterlockedXorAcquire##bits((data_type volatile *)ptr, arg);    \
      case memory_order::release:                                              \
        return InterlockedXorRelease##bits((data_type volatile *)ptr, arg);    \
      default:                                                                 \
        GPCL_ATOMIC_WARN_INVALID_MEMORDER(fetch_add);                          \
      case memory_order::acq_rel:                                              \
      case memory_order::seq_cst:                                              \
        return InterlockedXor##bits((data_type volatile *)ptr, arg);           \
      }                                                                        \
    }                                                                          \
  };

GPCL_DEFINE_WIN_ATOMIC_OPS(1, 8, char)
GPCL_DEFINE_WIN_ATOMIC_OPS(2, 16, short)
GPCL_DEFINE_WIN_ATOMIC_OPS(4, , long)
GPCL_DEFINE_WIN_ATOMIC_OPS(8, 64, __int64)

// Generic functions

template <typename T>
void win_atomic_store(T volatile *ptr, T val, memory_order order) noexcept
{
  win_atomic_ops<sizeof(T)>::store(ptr, val, order);
}

template <typename T>
T win_atomic_load(T const volatile *ptr, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::load(ptr, order);
}

template <typename T>
T win_atomic_exchange(T volatile *ptr, T desired, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::exchange(ptr, desired, order);
}

template <typename T>
bool win_atomic_exchange_strong(T volatile *ptr, T &expected, T desired,
                                memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::compare_exchange_strong(ptr, expected,
                                                            desired, order);
}

template <typename T>
bool win_atomic_exchange_strong(T volatile *ptr, T &expected, T desired,
                                memory_order success,
                                memory_order failure) noexcept
{
  return win_atomic_ops<sizeof(T)>::compare_exchange_strong(
      ptr, expected, desired, success, failure);
}

template <typename T>
bool win_atomic_exchange_weak(T volatile *ptr, T &expected, T desired,
                              memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::compare_exchange_weak(ptr, expected,
                                                          desired, order);
}

template <typename T>
bool win_atomic_exchange_weak(T volatile *ptr, T &expected, T desired,
                              memory_order success,
                              memory_order failure) noexcept
{
  return win_atomic_ops<sizeof(T)>::compare_exchange_weak(
      ptr, expected, desired, success, failure);
}

template <typename T>
T win_atomic_fetch_add(T volatile *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_add(ptr, arg, order);
}

template <typename T>
T win_atomic_fetch_sub(T volatile *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_sub(ptr, arg, order);
}

template <typename T>
T win_atomic_fetch_and(T volatile *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_and(ptr, arg, order);
}

template <typename T>
T win_atomic_fetch_or(T volatile *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_or(ptr, arg, order);
}

template <typename T>
T win_atomic_fetch_xor(T volatile *ptr, T arg, memory_order order) noexcept
{
  return win_atomic_ops<sizeof(T)>::fetch_xor(ptr, arg, order);
}

} // namespace detail
} // namespace gpcl

#endif
