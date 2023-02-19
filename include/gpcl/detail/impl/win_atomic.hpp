#ifndef GPCL_DETAIL_IMPL_WIN_ATOMIC_HPP
#define GPCL_DETAIL_IMPL_WIN_ATOMIC_HPP

#include <gpcl/detail/win_atomic.hpp>

namespace gpcl {
namespace detail {

template <typename T>
void win_atomic<T, 1>::store(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    WriteNoFence8(&value, desired);
    break;

  case memory_order::release:
  case memory_order::seq_cst:
    WriteRelease8(&value, desired);
    break;

  default:
    GPCL_UNREACHABLE("Invalid memory order for store operation");
  }
}

template <typename T>
T win_atomic<T, 1>::load(memory_order order) const volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return ReadNoFence8(&value);

  case memory_order::consume:
  case memory_order::acquire:
  case memory_order::seq_cst:
    return ReadAcquire8(&value);

  default:
    GPCL_UNREACHABLE("Invalid memory order for load operation");
  }
}

template <typename T>
T win_atomic<T, 1>::exchange(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeNoFence8(&value, desired);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAcquire8(&value, desired);

  case memory_order::release:
    return InterlockedExchangeRelease8(&value, desired);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchange8(&value, desired);
  }
}

template <typename T>
bool win_atomic<T, 1>::compare_exchange_strong(
    T &expected, T desired, memory_order order) volatile noexcept
{
  CHAR initial{};
  switch (order)
  {
  case memory_order::relaxed:
    initial = InterlockedCompareExchangeNoFence8(&value, desired, expected);
    break;

  case memory_order::consume:
  case memory_order::acquire:
    initial = InterlockedCompareExchangeAcquire8(&value, desired, expected);
    break;

  case memory_order::release:
    initial = InterlockedCompareExchangeRelease8(&value, desired, expected);
    break;

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    initial = InterlockedCompareExchange8(&value, desired, expected);
    break;
  }

  if (initial == expected)
    return true;

  expected = initial;
  return false;
}

template <typename T>
T win_atomic<T, 1>::fetch_add(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeAddNoFence8(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAddAcquire8(&value, arg);

  case memory_order::release:
    return InterlockedExchangeAddRelease8(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchangeAdd8(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 1>::fetch_and(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedAndNoFence8(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedAndAcquire8(&value, arg);

  case memory_order::release:
    return InterlockedAndRelease8(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedAnd8(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 1>::fetch_or(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedOrNoFence8(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedOrAcquire8(&value, arg);

  case memory_order::release:
    return InterlockedOrRelease8(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedOr8(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 1>::fetch_xor(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedXorNoFence8(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedXorAcquire8(&value, arg);

  case memory_order::release:
    return InterlockedXorRelease8(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedXor8(&value, arg);
  }
}

template <typename T>
void win_atomic<T, 1>::wait(T old, memory_order order) volatile noexcept
{
  while (load(order) == old)
    GPCL_VERIFY(WaitOnAddress(&value, &old, sizeof(T), INFINITE));
}

template <typename T>
void win_atomic<T, 1>::notify_one() volatile noexcept
{
  WakeByAddressSingle((LPVOID)&value);
}

template <typename T>
void win_atomic<T, 1>::notify_all() volatile noexcept
{
  WakeByAddressAll((LPVOID)&value);
}

/***********************************************************************************/

template <typename T>
void win_atomic<T, 2>::store(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    WriteNoFence16(&value, desired);
    break;

  case memory_order::release:
  case memory_order::seq_cst:
    WriteRelease16(&value, desired);
    break;

  default:
    GPCL_UNREACHABLE("Invalid memory order for store operation");
  }
}

template <typename T>
T win_atomic<T, 2>::load(memory_order order) const volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return ReadNoFence16(&value);

  case memory_order::consume:
  case memory_order::acquire:
  case memory_order::seq_cst:
    return ReadAcquire16(&value);

  default:
    GPCL_UNREACHABLE("Invalid memory order for load operation");
  }
}

template <typename T>
T win_atomic<T, 2>::exchange(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeNoFence16(&value, desired);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAcquire16(&value, desired);

  case memory_order::release:
    return InterlockedExchangeRelease16(&value, desired);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchange16(&value, desired);
  }
}

template <typename T>
bool win_atomic<T, 2>::compare_exchange_strong(
    T &expected, T desired, memory_order order) volatile noexcept
{
  CHAR initial{};
  switch (order)
  {
  case memory_order::relaxed:
    initial = InterlockedCompareExchangeNoFence16(&value, desired, expected);
    break;

  case memory_order::consume:
  case memory_order::acquire:
    initial = InterlockedCompareExchangeAcquire16(&value, desired, expected);
    break;

  case memory_order::release:
    initial = InterlockedCompareExchangeRelease16(&value, desired, expected);
    break;

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    initial = InterlockedCompareExchange16(&value, desired, expected);
    break;
  }

  if (initial == expected)
    return true;

  expected = initial;
  return false;
}

template <typename T>
T win_atomic<T, 2>::fetch_add(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeAddNoFence16(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAddAcquire16(&value, arg);

  case memory_order::release:
    return InterlockedExchangeAddRelease16(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchangeAdd16(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 2>::fetch_and(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedAndNoFence16(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedAndAcquire16(&value, arg);

  case memory_order::release:
    return InterlockedAndRelease16(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedAnd16(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 2>::fetch_or(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedOrNoFence16(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedOrAcquire16(&value, arg);

  case memory_order::release:
    return InterlockedOrRelease16(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedOr16(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 2>::fetch_xor(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedXorNoFence16(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedXorAcquire16(&value, arg);

  case memory_order::release:
    return InterlockedXorRelease16(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedXor16(&value, arg);
  }
}

template <typename T>
void win_atomic<T, 2>::wait(T old, memory_order order) volatile noexcept
{
  while (load(order) == old)
    GPCL_VERIFY(WaitOnAddress(&value, &old, sizeof(T), INFINITE));
}

template <typename T>
void win_atomic<T, 2>::notify_one() volatile noexcept
{
  WakeByAddressSingle((LPVOID)&value);
}

template <typename T>
void win_atomic<T, 2>::notify_all() volatile noexcept
{
  WakeByAddressAll((LPVOID)&value);
}

/*********************************************************************************/

template <typename T>
void win_atomic<T, 4>::store(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    WriteNoFence(&value, desired);
    break;

  case memory_order::release:
  case memory_order::seq_cst:
    WriteRelease(&value, desired);
    break;

  default:
    GPCL_UNREACHABLE("Invalid memory order for store operation");
  }
}

template <typename T>
T win_atomic<T, 4>::load(memory_order order) const volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return ReadNoFence(&value);

  case memory_order::consume:
  case memory_order::acquire:
  case memory_order::seq_cst:
    return ReadAcquire(&value);

  default:
    GPCL_UNREACHABLE("Invalid memory order for load operation");
  }
}

template <typename T>
T win_atomic<T, 4>::exchange(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeNoFence(&value, desired);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAcquire(&value, desired);

  case memory_order::release:
    return InterlockedExchangeRelease(&value, desired);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchange(&value, desired);
  }
}

template <typename T>
bool win_atomic<T, 4>::compare_exchange_strong(
    T &expected, T desired, memory_order order) volatile noexcept
{
  CHAR initial{};
  switch (order)
  {
  case memory_order::relaxed:
    initial = InterlockedCompareExchangeNoFence(&value, desired, expected);
    break;

  case memory_order::consume:
  case memory_order::acquire:
    initial = InterlockedCompareExchangeAcquire(&value, desired, expected);
    break;

  case memory_order::release:
    initial = InterlockedCompareExchangeRelease(&value, desired, expected);
    break;

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    initial = InterlockedCompareExchange(&value, desired, expected);
    break;
  }

  if (initial == expected)
    return true;

  expected = initial;
  return false;
}

template <typename T>
T win_atomic<T, 4>::fetch_add(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeAddNoFence(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAddAcquire(&value, arg);

  case memory_order::release:
    return InterlockedExchangeAddRelease(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchangeAdd(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 4>::fetch_and(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedAndNoFence(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedAndAcquire(&value, arg);

  case memory_order::release:
    return InterlockedAndRelease(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedAnd(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 4>::fetch_or(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedOrNoFence(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedOrAcquire(&value, arg);

  case memory_order::release:
    return InterlockedOrRelease(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedOr(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 4>::fetch_xor(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedXorNoFence(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedXorAcquire(&value, arg);

  case memory_order::release:
    return InterlockedXorRelease(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedXor(&value, arg);
  }
}

template <typename T>
void win_atomic<T, 4>::wait(T old, memory_order order) volatile noexcept
{
  while (load(order) == old)
    GPCL_VERIFY(WaitOnAddress(&value, &old, sizeof(T), INFINITE));
}

template <typename T>
void win_atomic<T, 4>::notify_one() volatile noexcept
{
  WakeByAddressSingle((LPVOID)&value);
}

template <typename T>
void win_atomic<T, 4>::notify_all() volatile noexcept
{
  WakeByAddressAll((LPVOID)&value);
}

/*******************************************************************************/

template <typename T>
void win_atomic<T, 8>::store(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    WriteNoFence64(&value, desired);
    break;

  case memory_order::release:
  case memory_order::seq_cst:
    WriteRelease64(&value, desired);
    break;

  default:
    GPCL_UNREACHABLE("Invalid memory order for store operation");
  }
}

template <typename T>
T win_atomic<T, 8>::load(memory_order order) const volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return ReadNoFence64(&value);

  case memory_order::consume:
  case memory_order::acquire:
  case memory_order::seq_cst:
    return ReadAcquire64(&value);

  default:
    GPCL_UNREACHABLE("Invalid memory order for load operation");
  }
}

template <typename T>
T win_atomic<T, 8>::exchange(T desired, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeNoFence64(&value, desired);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAcquire64(&value, desired);

  case memory_order::release:
    return InterlockedExchangeRelease64(&value, desired);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchange64(&value, desired);
  }
}

template <typename T>
bool win_atomic<T, 8>::compare_exchange_strong(
    T &expected, T desired, memory_order order) volatile noexcept
{
  CHAR initial{};
  switch (order)
  {
  case memory_order::relaxed:
    initial = InterlockedCompareExchangeNoFence64(&value, desired, expected);
    break;

  case memory_order::consume:
  case memory_order::acquire:
    initial = InterlockedCompareExchangeAcquire64(&value, desired, expected);
    break;

  case memory_order::release:
    initial = InterlockedCompareExchangeRelease64(&value, desired, expected);
    break;

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    initial = InterlockedCompareExchange64(&value, desired, expected);
    break;
  }

  if (initial == expected)
    return true;

  expected = initial;
  return false;
}

template <typename T>
T win_atomic<T, 8>::fetch_add(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedExchangeAddNoFence64(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedExchangeAddAcquire64(&value, arg);

  case memory_order::release:
    return InterlockedExchangeAddRelease64(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedExchangeAdd64(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 8>::fetch_and(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedAnd64NoFence(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedAnd64Acquire(&value, arg);

  case memory_order::release:
    return InterlockedAnd64Release(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedAnd64(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 8>::fetch_or(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedOr64NoFence(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedOr64Acquire(&value, arg);

  case memory_order::release:
    return InterlockedOr64Release(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedOr64(&value, arg);
  }
}

template <typename T>
T win_atomic<T, 8>::fetch_xor(T arg, memory_order order) volatile noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return InterlockedXor64NoFence(&value, arg);

  case memory_order::consume:
  case memory_order::acquire:
    return InterlockedXor64Acquire(&value, arg);

  case memory_order::release:
    return InterlockedXor64Release(&value, arg);

  case memory_order::acq_rel:
  case memory_order::seq_cst:
  default:
    return InterlockedXor64(&value, arg);
  }
}

template <typename T>
void win_atomic<T, 8>::wait(T old, memory_order order) volatile noexcept
{
  while (load(order) == old)
    GPCL_VERIFY(WaitOnAddress(&value, &old, sizeof(T), INFINITE));
}

template <typename T>
void win_atomic<T, 8>::notify_one() volatile noexcept
{
  WakeByAddressSingle((LPVOID)&value);
}

template <typename T>
void win_atomic<T, 8>::notify_all() volatile noexcept
{
  WakeByAddressAll((LPVOID)&value);
}

} // namespace detail
} // namespace gpcl

#endif
