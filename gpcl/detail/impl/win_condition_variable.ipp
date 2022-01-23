//
// win_condition_variable.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_CONDITION_VARIABLE_IPP
#define GPCL_DETAIL_IMPL_WIN_CONDITION_VARIABLE_IPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/win_condition_variable.hpp>
#include <gpcl/detail/win_mutex.hpp>
#include <gpcl/narrow_cast.hpp>

#include <synchapi.h>

namespace gpcl {
namespace detail {

win_condition_variable::win_condition_variable() noexcept
{
  ::InitializeConditionVariable(&cv_);
}

win_condition_variable::~win_condition_variable() noexcept {}

auto win_condition_variable::notify_one() -> void
{
  ::WakeConditionVariable(&cv_);
}

auto win_condition_variable::notify_all() -> void
{
  ::WakeAllConditionVariable(&cv_);
}

auto win_condition_variable::wait(unique_lock<win_mutex> &lock) -> void
{
  GPCL_ASSERT(lock.owns_lock());
  if (!::SleepConditionVariableCS(&cv_, lock.mutex().native_handle(), INFINITE))
    throw_system_error("SleepConditionVariableCS");
}

bool win_condition_variable::wait_until(unique_lock<win_mutex> &lock,
                                        const system_time &timeout_time)
{
  auto rel_time = timeout_time.elapsed();
  return wait_for(lock, rel_time);
}

bool win_condition_variable::wait_for(unique_lock<win_mutex> &lock,
                                      const duration &rel_time)
{
  GPCL_ASSERT(lock.owns_lock());
  const auto ms = rel_time.as_millis();
  if (!::SleepConditionVariableCS(&cv_, lock.mutex().native_handle(),
                                  narrow_cast<DWORD>(ms)))
  {
    DWORD dwError = ::GetLastError();
    if (dwError == ERROR_TIMEOUT)
      return true;
    throw_system_error(dwError, "SleepConditionVariableCS");
  }

  return false;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_CONDITION_VARIABLE_IPP
