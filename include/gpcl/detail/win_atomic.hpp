#ifndef GPCL_DETAIL_WIN_ATOMIC_HPP
#define GPCL_DETAIL_WIN_ATOMIC_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/atomic_facade.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/memory_order.hpp>

#include <type_traits>

#include <Windows.h>
#include <intrin.h>

#if defined _MSC_VER && !defined GPCL_SOURCE &&                                \
    !defined GPCL_DISABLE_AUTO_LINKING
#  pragma comment(lib, "Synchronization")
#endif

namespace gpcl {

template <typename T>
class shared_ptr;

template <typename T>
class weak_ptr;

namespace detail {

#if !(defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) ||  \
      defined(_M_ARM64EC))
#  define GPCL_DETAIL_WIN_ARM_MACHINE 0
#else
#  define GPCL_DETAIL_WIN_ARM_MACHINE 1
#endif

inline char InterlockedExchangeRelease8(volatile char *target,
                                        char value) noexcept
{
#if !gpcl_detail_win_arm_machine
  return InterlockedExchange8(target, value);
#else
  return _InterlockedExchange8_rel(target, value);
#endif
}

inline CHAR InterlockedCompareExchange8(volatile CHAR *destination,
                                        CHAR exchange, CHAR comparand) noexcept
{
  return _InterlockedCompareExchange8(destination, exchange, comparand);
}

inline CHAR InterlockedCompareExchangeNoFence8(volatile CHAR *destination,
                                               CHAR exchange,
                                               CHAR comparand) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedCompareExchange8(destination, exchange, comparand);
#else
  return _InterlockedCompareExchange8_nf(destination, exchange, comparand);
#endif
}

inline CHAR InterlockedCompareExchangeAcquire8(volatile CHAR *destination,
                                               CHAR exchange,
                                               CHAR comparand) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedCompareExchange8(destination, exchange, comparand);
#else
  return _InterlockedCompareExchange8_acq(destination, exchange, comparand);
#endif
}

inline CHAR InterlockedCompareExchangeRelease8(volatile CHAR *destination,
                                               CHAR exchange,
                                               CHAR comparand) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedCompareExchange8(destination, exchange, comparand);
#else
  return _InterlockedCompareExchange8_release(destination, exchange, comparand);
#endif
}

inline CHAR InterlockedExchangeAddNoFence8(CHAR volatile *addend,
                                           CHAR value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd8(addend, value);

#else
  return _InterlockedExchangeAdd8_nf(addend, value);
#endif
}

inline CHAR InterlockedExchangeAddAcquire8(CHAR volatile *addend,
                                           CHAR value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd8(addend, value);

#else
  return _InterlockedExchangeAdd8_acq(addend, value);
#endif
}

inline CHAR InterlockedExchangeAddRelease8(CHAR volatile *addend,
                                           CHAR value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd8(addend, value);

#else
  return _InterlockedExchangeAdd8_rel(addend, value);
#endif
}

inline SHORT InterlockedExchangeNoFence16(SHORT volatile *target,
                                          SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange16(target, value);
#else

  return _InterlockedExchange16_nf(target, value);
#endif
}

inline SHORT InterlockedExchangeAcquire16(SHORT volatile *target,
                                          SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange16(target, value);
#else

  return _InterlockedExchange16_acq(target, value);
#endif
}

inline SHORT InterlockedExchangeRelease16(SHORT volatile *target,
                                          SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange16(target, value);
#else

  return _InterlockedExchange16_rel(target, value);
#endif
}

inline SHORT InterlockedExchangeAdd16(SHORT volatile *addend,
                                      SHORT value) noexcept
{
  return _InterlockedExchangeAdd16(addend, value);
}

inline SHORT InterlockedExchangeAddNoFence16(SHORT volatile *addend,
                                             SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd16(addend, value);
#else
  return _InterlockedExchangeAdd16_nf(addend, value);
#endif
}

inline SHORT InterlockedExchangeAddAcquire16(SHORT volatile *addend,
                                             SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd16(addend, value);
#else
  return _InterlockedExchangeAdd16_acq(addend, value);
#endif
}

inline SHORT InterlockedExchangeAddRelease16(SHORT volatile *addend,
                                             SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd16(addend, value);
#else
  return _InterlockedExchangeAdd16_rel(addend, value);
#endif
}

inline SHORT InterlockedAndNoFence16(SHORT volatile *target,
                                     SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedAnd16(target, value);
#else
  return _InterlockedAnd16_nf(target, value);
#endif
}

inline SHORT InterlockedAndAcquire16(SHORT volatile *target,
                                     SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedAnd16(target, value);
#else
  return _InterlockedAnd16_acq(target, value);
#endif
}

inline SHORT InterlockedAndRelease16(SHORT volatile *target,
                                     SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedAnd16(target, value);
#else
  return _InterlockedAnd16_rel(target, value);
#endif
}

inline SHORT InterlockedOrNoFence16(SHORT volatile *target,
                                    SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedOr16(target, value);
#else
  return _InterlockedOr16_nf(target, value);
#endif
}

inline SHORT InterlockedOrAcquire16(SHORT volatile *target,
                                    SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedOr16(target, value);
#else
  return _InterlockedOr16_acq(target, value);
#endif
}

inline SHORT InterlockedOrRelease16(SHORT volatile *target,
                                    SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedOr16(target, value);
#else
  return _InterlockedOr16_rel(target, value);
#endif
}

inline SHORT InterlockedXorNoFence16(SHORT volatile *target,
                                     SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedXor16(target, value);
#else
  return _InterlockedXor16_nf(target, value);
#endif
}

inline SHORT InterlockedXorAcquire16(SHORT volatile *target,
                                     SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedXor16(target, value);
#else
  return _InterlockedXor16_acq(target, value);
#endif
}

inline SHORT InterlockedXorRelease16(SHORT volatile *target,
                                     SHORT value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedXor16(target, value);
#else
  return _InterlockedXor16_rel(target, value);
#endif
}

inline LONG InterlockedExchangeRelease(LONG volatile *target,
                                       LONG value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange(target, value);
#else
  return _InterlockedExchange_rel(target, value);
#endif
}

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_X64)

inline LONG InterlockedExchangeRelease64(LONG64 volatile *target,
                                         LONG64 value) noexcept
{
#  if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange64(target, value);
#  else
  return _InterlockedExchange64_rel(target, value);
#  endif
}
#endif

template <typename T, size_t Size = sizeof(T)>
struct win_atomic;

template <typename T>
struct win_atomic<T, 1> : atomic_facade<win_atomic<T, 1>, T>
{
  static constexpr bool is_always_lock_free = true;

  CHAR value;

  constexpr win_atomic() noexcept : value() {}

  constexpr win_atomic(T desired) noexcept : value(desired) {}

  bool is_lock_free() const volatile noexcept { return true; }

  void store(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T load(memory_order order = memory_order::seq_cst) const volatile noexcept;

  T exchange(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_add(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_and(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_or(T arg,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_xor(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  void wait(T old,
            memory_order order = memory_order::seq_cst) volatile noexcept;

  void notify_one() volatile noexcept;

  void notify_all() volatile noexcept;
};

template <typename T>
struct win_atomic<T, 2> : atomic_facade<win_atomic<T, 2>, T>
{
  static constexpr bool is_always_lock_free = true;

  SHORT value;

  constexpr win_atomic() noexcept : value() {}

  constexpr win_atomic(T desired) noexcept : value(desired) {}

  bool is_lock_free() const volatile noexcept { return true; }

  void store(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T load(memory_order order = memory_order::seq_cst) const volatile noexcept;

  T exchange(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_add(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_and(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_or(T arg,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_xor(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  void wait(T old,
            memory_order order = memory_order::seq_cst) volatile noexcept;

  void notify_one() volatile noexcept;

  void notify_all() volatile noexcept;
};

template <typename T>
struct win_atomic<T, 4> : atomic_facade<win_atomic<T, 4>, T>
{
  static constexpr bool is_always_lock_free = true;

  LONG value;

  constexpr win_atomic() noexcept : value() {}
  constexpr win_atomic(T desired) noexcept : value(desired) {}

  bool is_lock_free() const volatile noexcept { return true; }

  void store(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T load(memory_order order = memory_order::seq_cst) const volatile noexcept;

  T exchange(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_add(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_and(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_or(T arg,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_xor(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  void wait(T old,
            memory_order order = memory_order::seq_cst) volatile noexcept;

  void notify_one() volatile noexcept;

  void notify_all() volatile noexcept;
};

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_X64)

template <typename T>
struct win_atomic<T, 8> : atomic_facade<win_atomic<T, 8>, T>
{
  static constexpr bool is_always_lock_free = true;

  LONG64 value;

  constexpr win_atomic() noexcept : value() {}

  constexpr win_atomic(T desired) noexcept : value(desired) {}

  bool is_lock_free() const volatile noexcept { return true; }

  void store(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T load(memory_order order = memory_order::seq_cst) const volatile noexcept;

  T exchange(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_add(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_and(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_or(T arg,
             memory_order order = memory_order::seq_cst) volatile noexcept;

  T fetch_xor(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept;

  void wait(T old,
            memory_order order = memory_order::seq_cst) volatile noexcept;

  void notify_one() volatile noexcept;

  void notify_all() volatile noexcept;
};

#endif

} // namespace detail
} // namespace gpcl

#include <gpcl/detail/impl/win_atomic.hpp>

#endif //
