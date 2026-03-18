//
// posix_mutex.ipp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_MUTEX_IPP
#define GPCL_DETAIL_IMPL_POSIX_MUTEX_IPP

#include <gpcl/detail/posix_mutex.hpp>

#include <gpcl/debugstream.hpp>
#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/posix_clock.hpp>
#include <gpcl/detail/pthread_error.hpp>
#include <gpcl/strerror.hpp>

#include <pthread.h>

namespace gpcl {
namespace detail {

posix_mutex_attr::posix_mutex_attr()
{
  GPCL_THROW_IF_PTHREAD_FAILED(pthread_mutexattr_init(&attr_));
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
#if !defined(__CYGWIN__)
  const int b = static_cast<int>(prtl);
  int err = ::pthread_mutexattr_setprotocol(&attr_, b);
  GPCL_VERIFY(!err);
#endif
}

int posix_mutex_attr::priority_ceiling() const noexcept
{
#if !defined(__CYGWIN__)
  int prio = -1;
  int err = ::pthread_mutexattr_getprioceiling(&attr_, &prio);
  GPCL_VERIFY(!err);
  return prio;
#else
  return 0;
#endif
}

void posix_mutex_attr::priority_ceiling(int prio) noexcept
{
#if !defined(__CYGWIN__)
  int err = ::pthread_mutexattr_setprioceiling(&attr_, prio);
  GPCL_VERIFY(err);
#endif
}

posix_mutex_robust posix_mutex_attr::robust() const noexcept
{
#if !defined(__CYGWIN__)
  int r;
  int err = ::pthread_mutexattr_getrobust(&attr_, &r);
  GPCL_VERIFY(!err);
  return static_cast<posix_mutex_robust>(r);
#else
  return posix_mutex_robust();
#endif
}

void posix_mutex_attr::robust(posix_mutex_robust r) noexcept
{
#if !defined(__CYGWIN__)
  int err = ::pthread_mutexattr_setrobust(&attr_, static_cast<int>(r));
  GPCL_VERIFY(!err);
#endif
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
  GPCL_THROW_IF_PTHREAD_FAILED(::pthread_mutex_init(&mtx_, attr.get()));
}

posix_mutex_base::~posix_mutex_base()
{
  GPCL_VERIFY_0(pthread_mutex_destroy(&mtx_));
}

void posix_mutex_base::lock()
{
  GPCL_THROW_IF_PTHREAD_FAILED( pthread_mutex_lock(&mtx_) );
}

void posix_mutex_base::unlock()
{
  GPCL_THROW_IF_PTHREAD_FAILED( pthread_mutex_unlock(&mtx_) );
}

bool posix_mutex_base::try_lock()
{
  int err = pthread_mutex_trylock(&mtx_);
  if (err == EBUSY || err == EAGAIN)
    return false;
  if (err)
    GPCL_THROW_ERRNO(err, "Error when try_lock");
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
    GPCL_THROW_ERRNO(err, "pthread_mutex_timedlock");
  return false;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_MUTEX_IPP
