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
#include <gpcl/detail/error.hpp>
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
    throw_system_error(errc::resource_deadlock_would_occur, "win_mutex::lock");
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
                         "win_mutex::try_lock");
    }
    return true;
  }
  return false;
}

win_timed_mutex::win_timed_mutex()
    : mtx_(make_unique_resource_checked(CreateMutex(nullptr, false, nullptr),
                                        null_handle_deleter::invalid(),
                                        null_handle_deleter()))
{
  if (!mtx_)
    throw_system_error("CreateMutex");
}

auto win_timed_mutex::lock() -> void
{
  switch (WaitForSingleObject(mtx_.get(), INFINITE))
  {
  case WAIT_OBJECT_0:
    return;

  case WAIT_ABANDONED:
    throw_system_error(errc::owner_dead, "win_timed_mutex::lock");

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

auto gpcl::detail::win_timed_mutex::unlock() -> void
{
  if (!::ReleaseMutex(mtx_.get()))
    throw_system_error("ReleaseMutex");
}

auto win_timed_mutex::try_lock() -> bool
{
  switch (WaitForSingleObject(mtx_.get(), 0))
  {
  case WAIT_OBJECT_0:
    return true;

  case WAIT_TIMEOUT:
    return false;

  case WAIT_ABANDONED:
    throw_system_error(errc::owner_dead, "win_timed_mutex::try_lock");

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

auto win_timed_mutex::try_lock_for(system_clock::duration dur) -> bool
{
  switch (WaitForSingleObject(
      mtx_.get(),
      narrow_cast<DWORD>(
          chrono::duration_cast<chrono::milliseconds>(dur).count())))
  {
  case WAIT_OBJECT_0:
    return true;

  case WAIT_TIMEOUT:
    return false;

  case WAIT_ABANDONED:
    throw_system_error(errc::owner_dead, "win_timed_mutex::try_lock_for");

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

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
