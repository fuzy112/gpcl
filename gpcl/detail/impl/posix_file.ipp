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

expected<void, error_code> posix_file::open(create_only_t, czstring<> path,
                                            access_mode mode)
{
  fd_ = ::open(path, O_CREAT | O_EXCL | O_CLOEXEC | mode, 0666);
  if (fd_ == -1)
  {
    return unexpected<error_code>{in_place, errno, generic_category()};
  }
  return {};
}

expected<void, error_code> posix_file::open(open_only_t, czstring<> path,
                                            access_mode mode)
{
  fd_ = ::open(path, O_CLOEXEC | mode);
  if (fd_ == -1)
  {
    return unexpected<error_code>{in_place, errno, generic_category()};
  }
  return {};
}

expected<void, error_code> posix_file::open(open_or_create_t,
                                                 czstring<> path,
                                                 access_mode mode)
{
  fd_ = ::open(path, O_CREAT | O_CLOEXEC | mode, 0666);
  if (fd_ == -1)
  {
    return unexpected<error_code>{in_place, errno, generic_category()};
  }
  return {};
}

expected<void, error_code> posix_file::close()
{
  if (::close(fd_) == -1)
  {
    return unexpected<error_code>(in_place, errno, generic_category());
  }

  fd_ = -1;
  return {};
}

expected<posix_file, error_code> posix_file::clone()
{
  if (fd_ == -1)
  {
    return unexpected(std::make_error_code(std::errc::bad_file_descriptor));
  }

  auto fd2 = ::dup(fd_);
  if (fd2 == -1)
  {
    return unexpected<error_code>(in_place, errno, generic_category());
  }

  return posix_file{fd2};
}

expected<std::size_t, error_code> posix_file::tell()
{
  off_t ret = ::lseek(fd_, 0, SEEK_CUR);
  if (ret == -1)
    return unexpected<error_code>{in_place, errno, generic_category()};
  return ret;
}

expected<void, error_code> posix_file::seek(offset_type off, seek_direction dir)
{
  off_t ret = ::lseek(fd_, off, dir);
  if (ret == -1)
  {
    return unexpected<error_code>{in_place, errno, generic_category()};
  }
  return {};
}

expected<void, error_code> posix_file::truncate(std::size_t size)
{
  // no need to use call ftruncate64 explicitly. the glibc wrapper handles this.
  if (::ftruncate(fd_, size) == -1)
  {
    return unexpected<error_code>{in_place, errno, generic_category()};
  }
  return {};
}

expected<void, error_code> posix_file::unlink(czstring<> filename)
{
  if (::unlink(filename) == -1)
    return unexpected<error_code>{in_place, errno, generic_category()};
  return {};
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_FILE_IPP
