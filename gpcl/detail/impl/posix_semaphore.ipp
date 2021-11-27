//
// posix_semaphore.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/posix_semaphore.hpp>

#ifdef GPCL_POSIX
#  include <semaphore.h>

namespace gpcl {
namespace detail {

posix_semaphore::posix_semaphore(unsigned int init_value)
{
  if (-1 == ::sem_init(&sem_, false, init_value))
    throw_system_error("sem_init");
}

posix_semaphore::~posix_semaphore()
{
  GPCL_VERIFY(0 == ::sem_destroy(&sem_));
}

unsigned int posix_semaphore::get_value()
{
  int sval;
  if (-1 == ::sem_getvalue(&sem_, &sval))
    throw_system_error("sem_getvalue");

  if (sval < 0)
    return 0;
  return sval;
}

void posix_semaphore::post()
{
  if (-1 == ::sem_post(&sem_))
    throw_system_error("sem_post");
}

void posix_semaphore::wait()
{
  if (-1 == ::sem_wait(&sem_))
    throw_system_error("sem_wait");
}

bool posix_semaphore::try_wait()
{
  if (-1 == ::sem_trywait(&sem_))
  {
    if (errno == EWOULDBLOCK)
      return false;
    throw_system_error("sem_trywait");
  }
  return true;
}

} // namespace detail
} // namespace gpcl

#endif
