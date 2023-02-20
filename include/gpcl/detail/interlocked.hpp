#ifndef GPCL_DETAIL_WIN_INTERLOCKED_HPP
#define GPCL_DETAIL_WIN_INTERLOCKED_HPP

#include <gpcl/detail/config.hpp>

#include <intrin.h>

#if defined _MSC_VER && !defined GPCL_SOURCE &&                                \
    !defined GPCL_DISABLE_AUTO_LINKING
#  pragma comment(lib, "Synchronization")
#endif

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

} // namespace detail
} // namespace gpcl

#endif //
