//
// posix_lock_file.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/detail/posix_lock_file.hpp>
#include <gpcl/unique_lock.hpp>
#include <fcntl.h>
#include <unistd.h>

namespace gpcl {
namespace detail {

void posix_lock_file::lock()
{
  unique_lock<mutex_type> lk{mtx_};

  GPCL_ASSERT(!owns_lock());
  file_.open(open_or_create, filename_.c_str(),
             posix_file::access_mode::readwrite).value();

  // Lock the whole file.
  if (::lockf(file_.native_handle(), F_LOCK, 0) == -1)
  {
    throw_system_error(__func__);
  }

  write_pid();

  owns_lock_ = true;
}

bool posix_lock_file::try_lock()
{
  unique_lock<mutex_type> lk{mtx_};

  GPCL_ASSERT(!owns_lock());
  file_.open(open_or_create, filename_.c_str(),
             posix_file::access_mode::readwrite).value();

  // try locking the file.
  if (lockf(file_.native_handle(), F_TLOCK, 0) == -1)
  {
    int err = errno;
    if (err == EACCES || err == EAGAIN)
    {
      return false;
    }

    throw_system_error(__func__);
  }

  write_pid();

  owns_lock_ = true;
  return true;
}

void posix_lock_file::write_pid()
{
  // truncate the file size to zero.
  file_.truncate(0).value();

  // write PID.
  auto pid_str = std::to_string(getpid());
  file_.write_some(gpcl::buffer(pid_str)).value();
}

void posix_lock_file::unlock()
{
  unique_lock<mutex_type> lk{mtx_};

  GPCL_ASSERT(owns_lock());

  // Remove the file.
  // At the moment other processes can acquire the lock.
  posix_file::unlink(filename_.c_str()).value();

  // truncate the file.
  file_.truncate(0).value();

  // unlock the file.
  if (lockf(file_.native_handle(), F_ULOCK, 0) == -1)
  {
    throw_system_error(__func__);
  }

  // close the file descriptor.
  file_.close().value();

  owns_lock_ = false;
}

} // namespace detail
} // namespace gpcl