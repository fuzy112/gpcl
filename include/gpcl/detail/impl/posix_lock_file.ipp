//
// posix_lock_file.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_LOCK_FILE_IPP
#define GPCL_DETAIL_IMPL_POSIX_LOCK_FILE_IPP

#include <gpcl/buffer.hpp>
#include <gpcl/detail/posix_lock_file.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/unique_lock.hpp>

#include <fcntl.h>
#include <unistd.h>

namespace gpcl {
namespace detail {

void posix_lock_file::lock()
{
  unique_lock<mutex_type> lk{mtx_};

  GPCL_ASSERT(!owns_lock());

  GPCL_THROW_LAST_ERROR_IF(
      !(fd_ = unique_fd(
            open(filename_.c_str(), O_CREAT | O_CLOEXEC | O_RDWR, 0644))));

  // Lock the whole file.
  GPCL_THROW_LAST_ERROR_IF(lockf(fd_.get(), F_LOCK, 0) < 0);

  write_pid();

  owns_lock_ = true;
}

bool posix_lock_file::try_lock()
{
  unique_lock<mutex_type> lk{mtx_};

  GPCL_ASSERT(!owns_lock());
  GPCL_THROW_LAST_ERROR_IF(
      !(fd_ = unique_fd(
            open(filename_.c_str(), O_CREAT | O_CLOEXEC | O_RDWR, 0644))));

  // try locking the file.
  if (lockf(fd_.get(), F_TLOCK, 0) < 0)
  {
    if (errno == EACCES || errno == EAGAIN)
      return false;

    GPCL_THROW_ERRNO(errno, "lockf failed");
  }

  write_pid();

  owns_lock_ = true;
  return true;
}

void posix_lock_file::write_pid()
{
  // truncate the file size to zero.
  GPCL_THROW_LAST_ERROR_IF(ftruncate(fd_.get(), 0) < 0);

  // write PID.
  auto pid_str = std::to_string(getpid());
  GPCL_THROW_LAST_ERROR_IF(write(fd_.get(), pid_str.data(), pid_str.size()) <
                           0);
}

void posix_lock_file::unlock()
{
  unique_lock<mutex_type> lk{mtx_};

  GPCL_ASSERT(owns_lock());

  // Remove the file.
  // At the moment other processes can acquire the lock.
  // posix_file::unlink(filename_.c_str()).value();

  // truncate the file.
  GPCL_THROW_LAST_ERROR_IF(ftruncate(fd_.get(), 0) < 0);

  // unlock the file.
  GPCL_THROW_LAST_ERROR_IF(lockf(fd_.get(), F_ULOCK, 0) < 0);

  // close the file
  fd_.reset();

  owns_lock_ = false;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_LOCK_FILE_IPP
