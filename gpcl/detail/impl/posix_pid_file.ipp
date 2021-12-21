//
// posix_pid_file.ipp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_PID_FILE_IPP
#define GPCL_DETAIL_IMPL_PID_FILE_IPP

#include <gpcl/detail/posix_pid_file.hpp>
#include <gpcl/error.hpp>
#include <gpcl/time.hpp>

#include <climits>
#include <libgen.h>
#include <sys/inotify.h>

namespace gpcl {
namespace detail {

posix_pid_file::posix_pid_file(std::string path)
    : path_(std::move(path)),
      f_(open_or_create, path_.c_str(), file::readwrite)
{
  GPCL_ASSERT(f_.is_open());

  // lock the whole file.
  struct flock lockstr;
  lockstr.l_type = F_WRLCK;
  lockstr.l_whence = SEEK_SET;
  lockstr.l_start = 0;
  lockstr.l_len = 0;
  if (fcntl(f_.native_handle(), F_SETLK, &lockstr) == -1)
  {
    GPCL_THROW(system_error({errno, system_category()}, __func__));
  }

  // clear file content.
  f_.truncate(0).value();

  // write pid to the file
  std::string buf = std::to_string(getpid()) + '\n';
  GPCL_VERIFY(buf.length(), f_.write_some(gpcl::buffer(buf)));
}

posix_pid_file::~posix_pid_file() noexcept
{
  f_.unlink(path_.c_str()).value();
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_PID_FILE_IPP
