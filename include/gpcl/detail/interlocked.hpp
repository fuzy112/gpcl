//
// interlocked.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_INTERLOCKED_HPP
#define GPCL_DETAIL_WIN_INTERLOCKED_HPP

#include <gpcl/detail/config.hpp>

#include <intrin.h>
#include <windows.h>

#if defined _MSC_VER && !defined GPCL_SOURCE &&                                \
    !defined GPCL_DISABLE_AUTO_LINKING
#  pragma comment(lib, "Synchronization")
#endif

#if defined(_M_ARM) || defined(_M_ARM64)
#  define GPCL_DETAIL_WIN_ARM_MACHINE 1
#else
#  define GPCL_DETAIL_WIN_ARM_MACHINE 0
#endif

namespace gpcl {
namespace detail {

inline char InterlockedExchangeRelease8(volatile char *target,
                                        char value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange8(target, value);
#else
  return InterlockedExchange8_rel(target, value);
#endif
}

inline char InterlockedCompareExchange8(volatile char *destination,
                                        char exchange, char comparand) noexcept
{
  return _InterlockedCompareExchange8(destination, exchange, comparand);
}

inline char InterlockedCompareExchangeNoFence8(volatile char *destination,
                                               char exchange,
                                               char comparand) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedCompareExchange8(destination, exchange, comparand);
#else
  return _InterlockedCompareExchange8_nf(destination, exchange, comparand);
#endif
}

inline char InterlockedCompareExchangeAcquire8(volatile char *destination,
                                               char exchange,
                                               char comparand) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedCompareExchange8(destination, exchange, comparand);
#else
  return _InterlockedCompareExchange8_acq(destination, exchange, comparand);
#endif
}

inline char InterlockedCompareExchangeRelease8(volatile char *destination,
                                               char exchange,
                                               char comparand) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedCompareExchange8(destination, exchange, comparand);
#else
  return _InterlockedCompareExchange8_rel(destination, exchange, comparand);
#endif
}

inline char InterlockedExchangeAddNoFence8(char volatile *addend,
                                           char value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd8(addend, value);

#else
  return _InterlockedExchangeAdd8_nf(addend, value);
#endif
}

inline char InterlockedExchangeAddAcquire8(char volatile *addend,
                                           char value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd8(addend, value);

#else
  return _InterlockedExchangeAdd8_acq(addend, value);
#endif
}

inline char InterlockedExchangeAddRelease8(char volatile *addend,
                                           char value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd8(addend, value);

#else
  return _InterlockedExchangeAdd8_rel(addend, value);
#endif
}

inline short InterlockedExchangeNoFence16(short volatile *target,
                                          short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange16(target, value);
#else

  return _InterlockedExchange16_nf(target, value);
#endif
}

inline short InterlockedExchangeAcquire16(short volatile *target,
                                          short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange16(target, value);
#else

  return _InterlockedExchange16_acq(target, value);
#endif
}

inline short InterlockedExchangeRelease16(short volatile *target,
                                          short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange16(target, value);
#else

  return _InterlockedExchange16_rel(target, value);
#endif
}

inline short InterlockedExchangeAdd16(short volatile *addend,
                                      short value) noexcept
{
  return _InterlockedExchangeAdd16(addend, value);
}

inline short InterlockedExchangeAddNoFence16(short volatile *addend,
                                             short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd16(addend, value);
#else
  return _InterlockedExchangeAdd16_nf(addend, value);
#endif
}

inline short InterlockedExchangeAddAcquire16(short volatile *addend,
                                             short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd16(addend, value);
#else
  return _InterlockedExchangeAdd16_acq(addend, value);
#endif
}

inline short InterlockedExchangeAddRelease16(short volatile *addend,
                                             short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchangeAdd16(addend, value);
#else
  return _InterlockedExchangeAdd16_rel(addend, value);
#endif
}

inline short InterlockedAndNoFence16(short volatile *target,
                                     short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedAnd16(target, value);
#else
  return _InterlockedAnd16_nf(target, value);
#endif
}

inline short InterlockedAndAcquire16(short volatile *target,
                                     short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedAnd16(target, value);
#else
  return _InterlockedAnd16_acq(target, value);
#endif
}

inline short InterlockedAndRelease16(short volatile *target,
                                     short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedAnd16(target, value);
#else
  return _InterlockedAnd16_rel(target, value);
#endif
}

inline short InterlockedOrNoFence16(short volatile *target,
                                    short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedOr16(target, value);
#else
  return _InterlockedOr16_nf(target, value);
#endif
}

inline short InterlockedOrAcquire16(short volatile *target,
                                    short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedOr16(target, value);
#else
  return _InterlockedOr16_acq(target, value);
#endif
}

inline short InterlockedOrRelease16(short volatile *target,
                                    short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedOr16(target, value);
#else
  return _InterlockedOr16_rel(target, value);
#endif
}

inline short InterlockedXorNoFence16(short volatile *target,
                                     short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedXor16(target, value);
#else
  return _InterlockedXor16_nf(target, value);
#endif
}

inline short InterlockedXorAcquire16(short volatile *target,
                                     short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedXor16(target, value);
#else
  return _InterlockedXor16_acq(target, value);
#endif
}

inline short InterlockedXorRelease16(short volatile *target,
                                     short value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedXor16(target, value);
#else
  return _InterlockedXor16_rel(target, value);
#endif
}

inline long InterlockedExchangeRelease(long volatile *target,
                                       long value) noexcept
{
#if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange(target, value);
#else
  return _InterlockedExchange_rel(target, value);
#endif
}

#if defined(_WIN64)

inline __int64 InterlockedExchangeRelease64(__int64 volatile *target,
                                            __int64 value) noexcept
{
#  if !GPCL_DETAIL_WIN_ARM_MACHINE
  return InterlockedExchange64(target, value);
#  else
  return _InterlockedExchange64_rel(target, value);
#  endif
}

#  define InterlockedAndAcquire64 InterlockedAnd64Acquire
#  define InterlockedAndNoFence64 InterlockedAnd64NoFence
#  define InterlockedAndRelease64 InterlockedAnd64Release
#  define InterlockedOrNoFence64 InterlockedOr64NoFence
#  define InterlockedOrAcquire64  InterlockedOr64Acquire
#  define InterlockedOrRelease64 InterlockedOr64Release
#  define InterlockedXorAcquire64 InterlockedXor64Acquire
#  define InterlockedXorNoFence64 InterlockedXor64NoFence
#  define InterlockedXorRelease64 InterlockedXor64Release

#endif

} // namespace detail
} // namespace gpcl

#endif //
