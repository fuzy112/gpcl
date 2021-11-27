//
// posix_condition_variable.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/detail/error.hpp>
#include <gpcl/detail/posix_clock.hpp>
#include <gpcl/detail/posix_condition_variable.hpp>
#include <gpcl/detail/posix_mutex.hpp>
#include <type_traits>

#ifdef GPCL_POSIX
#  include <pthread.h>

namespace gpcl {
namespace detail {
posix_condition_variable::posix_condition_variable()
{
  int err = pthread_cond_init(&cond_, nullptr);
  if (err)
    throw_system_error(err, "pthread_cond_init");
}

posix_condition_variable::~posix_condition_variable()
{
  int err = pthread_cond_destroy(&cond_);
  GPCL_ASSERT(err == 0); // "failed to destroy the condition variable"
}

void posix_condition_variable::wait(gpcl::unique_lock<posix_normal_mutex> &lock)
{
  GPCL_ASSERT(lock.owns_lock());
  int err = pthread_cond_wait(&cond_, lock.mutex().native_handle());
  if (err == 0 || err == EAGAIN)
    return;

  throw_system_error(err, "pthread_cond_wait");
}

// returns false: no timeout
// returns true: timeout
bool posix_condition_variable::wait_until(
    gpcl::unique_lock<posix_normal_mutex> &lock,
    const system_time &timeout_time)
{
  GPCL_ASSERT(lock.owns_lock());
  const auto ts = timeout_time.duration_since_epoch().to_timespec();
  int err = ::pthread_cond_timedwait(&cond_, lock.mutex().native_handle(), &ts);

  if (err == ETIMEDOUT)
    return true;

  if (err)
    throw_system_error(err, "pthread_cond_timedwait");

  return false;
}

bool posix_condition_variable::wait_for(
    gpcl::unique_lock<posix_normal_mutex> &lock, const duration &rel_time)
{
  return wait_until(lock, system_time::now().checked_add(rel_time));
}

void posix_condition_variable::notify_one()
{
  int err = pthread_cond_signal(&cond_);
  if (err)
    throw_system_error(err, "pthread_cond_signal");
}

void posix_condition_variable::notify_all()
{
  int err = pthread_cond_broadcast(&cond_);
  if (err)
    throw_system_error(err, "pthread_cond_broadcast");
}

} // namespace detail
} // namespace gpcl

#endif
