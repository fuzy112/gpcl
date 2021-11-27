//
// posix_mutex.ipp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/posix_clock.hpp>
#include <gpcl/detail/posix_mutex.hpp>

#ifdef GPCL_POSIX
#  include <pthread.h>

namespace gpcl {
namespace detail {

posix_mutex_attr::posix_mutex_attr()
{
  int err = ::pthread_mutexattr_init(&attr_);
  if (err)
    throw_system_error(err, "pthread_mutexattr_init");
}

posix_mutex_attr::~posix_mutex_attr() noexcept
{
  int err = ::pthread_mutexattr_destroy(&attr_);
  GPCL_VERIFY(!err);
}

posix_mutex_protocol posix_mutex_attr::protocol() const noexcept
{
  int prtl = -1;
  int err = ::pthread_mutexattr_getprotocol(&attr_, &prtl);
  GPCL_VERIFY(!err);
  return static_cast<posix_mutex_protocol>(prtl);
}

void posix_mutex_attr::protocol(posix_mutex_protocol prtl) noexcept
{
  const int b = static_cast<int>(prtl);
  int err = ::pthread_mutexattr_setprotocol(&attr_, b);
  GPCL_VERIFY(!err);
}

int posix_mutex_attr::priority_ceiling() const noexcept
{
  int prio = -1;
  int err = ::pthread_mutexattr_getprioceiling(&attr_, &prio);
  GPCL_VERIFY(!err);
  return prio;
}

void posix_mutex_attr::priority_ceiling(int prio) noexcept
{
  int err = ::pthread_mutexattr_setprioceiling(&attr_, prio);
  GPCL_VERIFY(err);
}

posix_mutex_robust posix_mutex_attr::robust() const noexcept
{
  int r;
  int err = ::pthread_mutexattr_getrobust(&attr_, &r);
  GPCL_VERIFY(!err);
  return static_cast<posix_mutex_robust>(r);
}

void posix_mutex_attr::robust(posix_mutex_robust r) noexcept
{
  int err = ::pthread_mutexattr_setrobust(&attr_, static_cast<int>(r));
  GPCL_VERIFY(!err);
}

posix_mutex_type posix_mutex_attr::type() const noexcept
{
  int t;
  int err = ::pthread_mutexattr_gettype(&attr_, &t);
  GPCL_VERIFY(!err);
  return static_cast<posix_mutex_type>(t);
}

void posix_mutex_attr::type(posix_mutex_type t) noexcept
{
  int err = ::pthread_mutexattr_settype(&attr_, static_cast<int>(t));
  GPCL_VERIFY(!err);
}

posix_mutex_base::posix_mutex_base(const posix_mutex_attr &attr)
{
  int err = ::pthread_mutex_init(&mtx_, attr.get());
  if (err)
    throw_system_error(err, "pthread_mutex_init");
}

posix_mutex_base::~posix_mutex_base()
{
  int err = pthread_mutex_destroy(&mtx_);
  if (err)
    print_error(err, "pthread_mutex_destroy");
}

void posix_mutex_base::lock()
{
  int err = pthread_mutex_lock(&mtx_);
  if (err)
    throw_system_error(err, "pthread_mutex_lock");
}

void posix_mutex_base::unlock()
{
  int err = pthread_mutex_unlock(&mtx_);
  if (err)
    throw_system_error(err, "pthread_mutex_unlock");
}

bool posix_mutex_base::try_lock()
{
  int err = pthread_mutex_trylock(&mtx_);
  if (err == EBUSY || err == EAGAIN)
    return false;
  if (err)
    throw_system_error(err, "pthread_mutex_trylock");
  return true;
}

bool posix_mutex_base::try_lock_for(realtime_clock::duration dur)
{
  return try_lock_until(realtime_clock::now() + dur);
}

bool posix_mutex_base::try_lock_until(realtime_clock::time_point tp)
{
  const auto ts = to_timespec(tp.time_since_epoch());
  int err = pthread_mutex_timedlock(&mtx_, &ts);
  if (err == ETIMEDOUT)
    return false;
  if (err)
    throw_system_error(err, "pthread_mutex_timedlock");
  return false;
}

} // namespace detail
} // namespace gpcl

#endif
