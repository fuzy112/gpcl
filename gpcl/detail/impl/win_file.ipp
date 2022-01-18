//
// win_file.ipp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/detail/error.hpp>
#include <gpcl/detail/win_file.hpp>
#include <gpcl/narrow_cast.hpp>

namespace gpcl {
namespace detail {

void win_file::open(open_only_t, cwzstring<> filename, access_mode mode,
                    error_code &error)
{
  DWORD share_mode = 0;
  hd_ = ::CreateFile2(filename, mode, share_mode, OPEN_EXISTING, nullptr);
  if (hd_ == INVALID_HANDLE_VALUE)
  {
    error = error_code{narrow<int>(GetLastError()), system_category()};
    return;
  }

  error.clear();
}

void win_file::open(create_only_t, cwzstring<> filename, access_mode mode,
                    error_code &error)
{
  DWORD share_mode = 0;
  hd_ = ::CreateFile2(filename, mode, share_mode, CREATE_NEW, nullptr);
  if (hd_ == INVALID_HANDLE_VALUE)
  {
    error = error_code{narrow<int>(GetLastError()), system_category()};
    return;
  }

  error.clear();
}

void win_file::open(open_or_create_t, cwzstring<> filename, access_mode mode,
                    error_code &error)
{
  DWORD share_mode = 0;
  hd_ = ::CreateFile2(filename, mode, share_mode, OPEN_ALWAYS, nullptr);
  if (hd_ == INVALID_HANDLE_VALUE)
  {
    error = error_code{narrow<int>(GetLastError()), system_category()};
    return;
  }

  error.clear();
}

void win_file::unlink(cwzstring<> filename, error_code &error)
{
  if (!DeleteFileW(filename))
  {
    error = error_code{narrow<int>(GetLastError()), system_category()};
    return;
  }

  error.clear();
}

} // namespace detail
} // namespace gpcl
