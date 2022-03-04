//
// win_mutex.ipp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_MUTEX_IPP
#define GPCL_DETAIL_IMPL_WIN_MUTEX_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/win_mutex.hpp>
#include <gpcl/narrow_cast.hpp>

#include <synchapi.h>

namespace gpcl {
namespace detail {

void win_recursive_mutex::init() noexcept
{
  call_once(once_, InitializeCriticalSection, &cs_);
}

win_recursive_mutex::~win_recursive_mutex() noexcept
{
  if (once_.initialized())
    DeleteCriticalSection(&cs_);
}

auto win_recursive_mutex::lock() noexcept -> void
{
  init();
  EnterCriticalSection(&cs_);
}

auto win_mutex::lock() -> void
{
  win_recursive_mutex::lock();
  if (native_handle()->RecursionCount > 1)
  {
    win_recursive_mutex::unlock();
    throw_system_error(errc::resource_deadlock_would_occur, "win_mutex::lock",
                       GPCL_SOURCE_LOCATION_CURRENT_LINE());
  }
}

auto win_recursive_mutex::unlock() noexcept -> void
{
  LeaveCriticalSection(&cs_);
}

auto win_mutex::unlock() -> void
{
  win_recursive_mutex::unlock();
}

auto win_recursive_mutex::try_lock() noexcept -> bool
{
  init();
  return TryEnterCriticalSection(&cs_);
}

auto win_mutex::try_lock() -> bool
{
  if (win_recursive_mutex::try_lock())
  {
    if (native_handle()->RecursionCount > 1)
    {
      win_recursive_mutex::unlock();
      throw_system_error(errc::resource_deadlock_would_occur,
                         "win_mutex::try_lock",
                         GPCL_SOURCE_LOCATION_CURRENT_LINE());
    }
    return true;
  }
  return false;
}

win_timed_mutex::win_timed_mutex()
{
  mtx_.reset(::CreateMutex(nullptr, false, nullptr));
  GPCL_THROW_LAST_ERROR_IF(!mtx_);
}

auto win_timed_mutex::lock() -> void
{
  DWORD wait_result;
  GPCL_THROW_LAST_ERROR_IF(
      (wait_result = WaitForSingleObject(mtx_.get(), INFINITE)) == WAIT_FAILED);

  switch (wait_result)
  {
  case WAIT_OBJECT_0:
    return;

  case WAIT_ABANDONED:
    throw_system_error(errc::owner_dead, "WaitForSingleObject",
                       GPCL_SOURCE_LOCATION_CURRENT_LINE());

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

auto gpcl::detail::win_timed_mutex::unlock() -> void
{
  GPCL_THROW_LAST_ERROR_IF(!::ReleaseMutex(mtx_.get()));
}

auto win_timed_mutex::try_lock() -> bool
{
  DWORD wait_result;
  GPCL_THROW_LAST_ERROR_IF((wait_result = WaitForSingleObject(mtx_.get(), 0)) ==
                           WAIT_FAILED);

  switch (wait_result)
  {
  case WAIT_OBJECT_0:
    return true;

  case WAIT_TIMEOUT:
    return false;

  case WAIT_ABANDONED:
    throw_system_error(errc::owner_dead, "win_timed_mutex::try_lock",
                       GPCL_SOURCE_LOCATION_CURRENT_LINE());

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

auto win_timed_mutex::try_lock_for(system_clock::duration dur) -> bool
{
  DWORD wait_result;
  GPCL_THROW_LAST_ERROR_IF(
      (wait_result = WaitForSingleObject(
           mtx_.get(),
           narrow_cast<DWORD>(
               chrono::duration_cast<chrono::milliseconds>(dur).count()))) ==
      WAIT_FAILED);

  switch (wait_result)
  {
  case WAIT_OBJECT_0:
    return true;

  case WAIT_TIMEOUT:
    return false;

  case WAIT_ABANDONED:
    throw_system_error(errc::owner_dead, "win_timed_mutex::try_lock_for",
                       GPCL_SOURCE_LOCATION_CURRENT_LINE());

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

auto win_timed_mutex::try_lock_until(system_clock::time_point tp) -> bool
{
  const auto dur = tp - system_clock::now();
  if (dur > system_clock::duration(0))
    return try_lock_for(dur);
  else
    return try_lock();
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_MUTEX_IPP
