//
// posix_semaphore.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_SEMAPHORE_IPP
#define GPCL_DETAIL_IMPL_POSIX_SEMAPHORE_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/posix_semaphore.hpp>
#include <gpcl/detail/throw_system_error.hpp>

#include <semaphore.h>

namespace gpcl {
namespace detail {

posix_semaphore::posix_semaphore(unsigned int init_value)
{
  GPCL_THROW_LAST_ERROR_IF(sem_init(&sem_, false, init_value) < 0);
}

posix_semaphore::~posix_semaphore()
{
  GPCL_VERIFY(0 == ::sem_destroy(&sem_));
}

unsigned int posix_semaphore::get_value()
{
  int sval;

  GPCL_THROW_LAST_ERROR_IF(sem_getvalue(&sem_, &sval) < 0);

  if (sval < 0)
    return 0;
  return sval;
}

void posix_semaphore::post()
{
  GPCL_THROW_LAST_ERROR_IF(sem_post(&sem_) < 0);
}

void posix_semaphore::wait()
{
  GPCL_THROW_LAST_ERROR_IF(sem_wait(&sem_) < 0);
}

bool posix_semaphore::try_wait()
{
  if (-1 == ::sem_trywait(&sem_))
  {
    GPCL_THROW_LAST_ERROR_IF(errno != EWOULDBLOCK);
    return false;
  }
  return true;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_SEMAPHORE_IPP
