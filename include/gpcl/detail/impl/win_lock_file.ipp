//
// win_lock_file.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_LOCK_FILE_IPP
#define GPCL_DETAIL_IMPL_WIN_LOCK_FILE_IPP

#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/detail/win_lock_file.hpp>
#include <gpcl/narrow_cast.hpp>

#include <fileapi.h>

namespace gpcl {
namespace detail {

void win_lock_file::lock()
{
  while (!try_lock())
  {
    valid_handle hd(FindFirstChangeNotificationA(filename_.c_str(), FALSE,
                                                 FILE_NOTIFY_CHANGE_FILE_NAME));
    GPCL_TRY
    {
      GPCL_THROW_LAST_ERROR_IF(!hd);
      GPCL_THROW_LAST_ERROR_IF(WaitForSingleObject(hd.get(), INFINITE) ==
                               WAIT_FAILED);
    }
    GPCL_CATCH(system_error & e)
    {
      if (e.code() != error_code(ERROR_DIRECTORY, system_category()) &&
          e.code() != error_code(ERROR_FILE_NOT_FOUND, system_category()) &&
          e.code() != error_code(ERROR_ACCESS_DENIED, system_category()))
        GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }
}

bool win_lock_file::try_lock()
{
  GPCL_ASSERT(!owns_lock());

  handle_.reset(::CreateFileA(
      filename_.c_str(),
      GENERIC_READ | GENERIC_WRITE, // both read and write access
      FILE_SHARE_READ, // other process can only open this file for reading
      nullptr, // no security descriptor and child processes cannot inherit the
               // handle
      CREATE_NEW, // fail if already exists
      FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
      nullptr // no template file
      ));
  if (!handle_)
  {
    DWORD last_error = ::GetLastError();
    if (last_error == ERROR_FILE_EXISTS || last_error == ERROR_ACCESS_DENIED)
      return false;

    throw_last_error(last_error, "CreateFile",
                     GPCL_SOURCE_LOCATION_CURRENT_LINE());
  }
  return true;
}

void win_lock_file::unlock()
{
  GPCL_ASSERT(owns_lock());
  handle_.reset();
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_LOCK_FILE_IPP
