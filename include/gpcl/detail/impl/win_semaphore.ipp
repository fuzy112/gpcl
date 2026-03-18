//
// win_semaphore.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_SEMAPHORE_IPP
#define GPCL_DETAIL_IMPL_WIN_SEMAPHORE_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/win_semaphore.hpp>
#include <gpcl/narrow_cast.hpp>

#include <limits>

namespace gpcl {
namespace detail {

win_semaphore::win_semaphore(value_type init_value)
    : sem_(CreateSemaphoreA(nullptr, narrow_cast<LONG>(init_value),
                            narrow_cast<LONG>((max)()), nullptr))
{
  GPCL_THROW_LAST_ERROR_IF(!sem_);
}

bool win_semaphore::try_wait()
{
  DWORD wait_result;
  GPCL_THROW_LAST_ERROR_IF((wait_result = WaitForSingleObject(sem_.get(), 0)) ==
                           WAIT_FAILED);

  switch (wait_result)
  {
  case WAIT_OBJECT_0:
    return true;

  case WAIT_TIMEOUT:
    return false;

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

void win_semaphore::wait()
{
  DWORD wait_result;
  GPCL_THROW_LAST_ERROR_IF(
      (wait_result = WaitForSingleObject(sem_.get(), INFINITE)) == WAIT_FAILED);
  switch (wait_result)
  {
  case WAIT_OBJECT_0:
    return;

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

void win_semaphore::post()
{
  GPCL_THROW_LAST_ERROR_IF(!ReleaseSemaphore(sem_.get(), 1, nullptr));
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_SEMAPHORE_IPP
