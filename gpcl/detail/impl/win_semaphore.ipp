//
// win_semaphore.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_SEMAPHORE_IPP
#define GPCL_DETAIL_IMPL_WIN_SEMAPHORE_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/win_semaphore.hpp>
#include <gpcl/narrow_cast.hpp>

#include <limits>


namespace gpcl {
namespace detail {

win_semaphore::win_semaphore(value_type init_value)
    : sem_(make_unique_resource_checked(
          CreateSemaphoreA(nullptr, narrow_cast<LONG>(init_value),
                           narrow_cast<LONG>((max)()), nullptr),
          null_handle_deleter::invalid(), null_handle_deleter()))
{
  if (!sem_)
    throw_system_error("CreateSemaphoreA");
}

bool win_semaphore::try_wait()
{
  switch (WaitForSingleObject(sem_.get(), 0))
  {
  case WAIT_OBJECT_0:
    return true;

  case WAIT_TIMEOUT:
    return false;

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

void win_semaphore::wait()
{
  switch (WaitForSingleObject(sem_.get(), 0))
  {
  case WAIT_OBJECT_0:
    return;

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

void win_semaphore::post()
{
  if (!ReleaseSemaphore(sem_.get(), 1, nullptr))
    throw_system_error("ReleaseSemaphore");
}

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_WIN_SEMAPHORE_IPP
