//
// posix_pid_file.ipp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_PID_FILE_IPP
#define GPCL_DETAIL_IMPL_PID_FILE_IPP

#include <gpcl/detail/posix_pid_file.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/time.hpp>

#include <climits>
#include <libgen.h>

namespace gpcl {
namespace detail {

inline void lock_file_impl(int fd, bool unique, bool wait)
{
  GPCL_ASSERT(fd > 0);

  struct flock lockstr = {};
  lockstr.l_type = unique ? F_WRLCK : F_RDLCK;
  lockstr.l_whence = SEEK_SET;
  lockstr.l_start = 0;
  lockstr.l_len = 0;

  GPCL_THROW_LAST_ERROR_IF(::fcntl(fd, wait ? F_SETLKW : F_SETLK, &lockstr) <
                           0);
}

void posix_pid_file::lock_unique()
{
  lock_file_impl(f_.get(), true, false);
}

void posix_pid_file::lock_shared()
{
  lock_file_impl(f_.get(), false, false);
}

void posix_pid_file::lock_shared_wait()
{
  lock_file_impl(f_.get(), false, true);
}

posix_pid_file::posix_pid_file(std::string path, bool update_pid)
    : path_(std::move(path))
{
  if (update_pid)
    f_.reset(open(path_.c_str(), O_CLOEXEC | O_CREAT | O_RDWR, (mode_t)0644));
  else
    f_.reset(open(path_.c_str(), O_CLOEXEC | O_RDONLY));
  GPCL_THROW_LAST_ERROR_IF(!f_);

  if (update_pid)
    write_pid();
}

posix_pid_file::~posix_pid_file() noexcept
{
  if (owns_pid_)
  {
    GPCL_TRY { lock_unique(); }
    GPCL_CATCH(...) {}
    GPCL_CATCH_END
    unlink(path_.c_str());
  }
}

void posix_pid_file::do_write_pid(pid_t pid)
{
  char buf[40] = {0};
  snprintf(buf, sizeof(buf) - 1, "%d", pid);
  GPCL_THROW_LAST_ERROR_IF(::write(f_.get(), buf, strlen(buf)) < 0);
}

pid_t posix_pid_file::do_read_pid()
{
  char buf[40] = {0};
  GPCL_THROW_LAST_ERROR_IF(::read(f_.get(), buf, sizeof(buf) - 1) < 0);
  int pid = atoi(buf);
  if (pid <= 0)
    GPCL_THROW_ERRNO(EAGAIN, __func__);
  return pid;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_PID_FILE_IPP
