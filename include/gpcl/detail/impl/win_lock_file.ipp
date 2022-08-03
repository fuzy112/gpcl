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

constexpr auto FILE_LOCK_BYTES = UINT32_MAX;

void win_lock_file::lock()
{
  GPCL_ASSERT(!owns_lock());

  valid_handle file(
      ::CreateFileA(filename_.c_str(),
                    GENERIC_READ | GENERIC_WRITE, // both read and write access
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr, // no security descriptor and child processes
                             // cannot inherit the handle
                    OPEN_ALWAYS, FILE_ATTRIBUTE_TEMPORARY,
                    nullptr // no template file
                    ));
  GPCL_THROW_LAST_ERROR_IF(!file);

  OVERLAPPED overlapped = {};
  GPCL_THROW_LAST_ERROR_IF(!::LockFileEx(file.get(), LOCKFILE_EXCLUSIVE_LOCK, 0,
                                         FILE_LOCK_BYTES, 0, &overlapped));

  DWORD pid = GetCurrentProcessId();
  GPCL_THROW_LAST_ERROR_IF(SetFilePointer(file.get(), 0, NULL, FILE_BEGIN) ==
                           INVALID_SET_FILE_POINTER);
  GPCL_THROW_LAST_ERROR_IF(!SetEndOfFile(file.get()));

  std::string pid_str = std::to_string(pid);

  GPCL_THROW_LAST_ERROR_IF(
      !WriteFile(file.get(), pid_str.data(), narrow_cast<DWORD>(pid_str.size()), NULL, NULL));
  handle_ = std::move(file);
}

bool win_lock_file::try_lock()
{
  GPCL_ASSERT(!owns_lock());

  valid_handle file(
      ::CreateFileA(filename_.c_str(),
                    GENERIC_READ | GENERIC_WRITE, // both read and write access
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr, // no security descriptor and child processes
                             // cannot inherit the handle
                    OPEN_ALWAYS, FILE_ATTRIBUTE_TEMPORARY,
                    nullptr // no template file
                    ));
  GPCL_THROW_LAST_ERROR_IF(!file);

  OVERLAPPED overlapped = {};
  if (!::LockFileEx(file.get(),
                    LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, FILE_LOCK_BYTES,
                    0, &overlapped))
    return false;

  DWORD pid = GetCurrentProcessId();
  GPCL_THROW_LAST_ERROR_IF(SetFilePointer(handle_.get(), 0, NULL, FILE_BEGIN) ==
                           INVALID_SET_FILE_POINTER);
  GPCL_THROW_LAST_ERROR_IF(!SetEndOfFile(file.get()));

  std::string pid_str = std::to_string(pid);

  GPCL_THROW_LAST_ERROR_IF(
      !WriteFile(file.get(), pid_str.data(), narrow_cast<DWORD>(pid_str.size()), NULL, NULL));
  handle_ = std::move(file);

  return true;
}

void win_lock_file::unlock()
{
  GPCL_TRY
  {
    GPCL_ASSERT(owns_lock());
    GPCL_THROW_LAST_ERROR_IF(SetFilePointer(handle_.get(), 0, NULL, FILE_BEGIN) ==
                           INVALID_SET_FILE_POINTER);
    GPCL_THROW_LAST_ERROR_IF(!SetEndOfFile(handle_.get()));

    OVERLAPPED overlapped = {};
    GPCL_THROW_LAST_ERROR_IF(!UnlockFileEx(handle_.get(), 0, FILE_LOCK_BYTES, 0, &overlapped));
    handle_.reset();

    DeleteFileA(filename_.c_str());
  }
  GPCL_CATCH(...)
  {
    std::terminate();
  }
  GPCL_CATCH_END
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_LOCK_FILE_IPP
