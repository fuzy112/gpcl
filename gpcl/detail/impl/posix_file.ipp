//
// posix_file.ipp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_FILE_IPP
#define GPCL_DETAIL_IMPL_POSIX_FILE_IPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/posix_file.hpp>

#include <fcntl.h>
#include <unistd.h>

namespace gpcl {
namespace detail {

void posix_file::open(create_only_t, czstring<> path, access_mode mode,
                      error_code &error)
{
  fd_ = ::open(path, O_CREAT | O_EXCL | O_CLOEXEC | mode, 0666);
  if (fd_ == -1)
  {
    error.assign(errno, generic_category());
  }
  else
  {
    error = {};
  }
}

void posix_file::open(open_only_t, czstring<> path, access_mode mode,
                      error_code &error)
{
  fd_ = ::open(path, O_CLOEXEC | mode);
  if (fd_ == -1)
  {
    error.assign(errno, generic_category());
  }
  else
  {
    error = {};
  }
}

void posix_file::open(open_or_create_t, czstring<> path, access_mode mode,
                      error_code &error)
{
  fd_ = ::open(path, O_CREAT | O_CLOEXEC | mode, 0666);
  if (fd_ == -1)
  {
    error.assign(errno, generic_category());
  }
  else
  {
    error = {};
  }
}

void posix_file::close(error_code &error)
{
  if (fd_ != -1)
  {
    if (::close(fd_) == -1)
    {
      error.assign(errno, generic_category());
      return;
    }

    error = {};
    fd_ = -1;
  }
}

posix_file posix_file::clone(error_code &error)
{
  if (fd_ == -1)
  {
    error = std::make_error_code(std::errc::bad_file_descriptor);
    return {};
  }

  auto fd2 = ::dup(fd_);
  if (fd2 == -1)
  {
    error.assign(errno, generic_category());
    return {};
  }

  error = {};
  return posix_file{fd2};
}

std::size_t posix_file::tell(error_code &error)
{
  off_t ret = ::lseek(fd_, 0, SEEK_CUR);
  if (ret == -1)
    error = {errno, generic_category()};
  else
    error = {};
  return ret;
}

void posix_file::seek(offset_type off, seek_direction dir, error_code &error)
{
  off_t ret = ::lseek(fd_, off, dir);
  if (ret == -1)
  {
    error = {errno, generic_category()};
  }
  else
  {
    error = {};
  }
}

void posix_file::truncate(std::size_t size, error_code &error)
{
  // no need to use call ftruncate64 explicitly. the glibc wrapper handles this.
  if (::ftruncate(fd_, size) == -1)
  {
    error = {errno, generic_category()};
  }
  else
  {
    error = {};
  }
}

void posix_file::unlink(czstring<> filename, error_code &error)
{
  if (::unlink(filename) == -1)
    error = {errno, generic_category()};
  else
    error = {};
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_FILE_IPP
