//
// posix_file.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/detail/error.hpp>
#include <gpcl/detail/native_buffer_sequence.hpp>
#include <gpcl/detail/posix_file.hpp>
#include <unistd.h>

namespace gpcl {
namespace detail {

template <typename ConstBufferSequence>
std::size_t posix_file::write_some_at(offset_type off,
                                      const ConstBufferSequence &bs,
                                      error_code &error)
{
  native_buffer_sequence<ConstBufferSequence> nbs(bs);
  ssize_t nbytes = ::pwritev(fd_, nbs.iov.data(), nbs.iov.size(), off);
  if (nbytes == -1)
  {
    error = {errno, system_category()};
  }
  else
  {
    error = {};
  }
  return nbytes;
}

template <typename MutableBufferSequence>
std::size_t posix_file::read_some_at(offset_type off,
                                     const MutableBufferSequence &bs,
                                     error_code &error)
{
  native_buffer_sequence<MutableBufferSequence> nbs(bs);
  ssize_t nbytes = ::preadv64(fd_, nbs.iov.data(), nbs.iov.size(), off);
  if (nbytes == -1)
  {
    error = {errno, system_category()};
  }
  else
  {
    error = {};
  }
  return nbytes;
}

template <typename ConstBufferSequence>
std::size_t posix_file::write_some(const ConstBufferSequence &bs,
                                   error_code &error)
{
  native_buffer_sequence<ConstBufferSequence> nbs(bs);
  ssize_t nbytes = ::writev(fd_, nbs.iov.data(), nbs.iov.size());
  if (nbytes == -1)
  {
    error = {errno, system_category()};
  }
  else
  {
    error = {};
  }
  return nbytes;
}

template <typename MutableBufferSequence>
std::size_t posix_file::read_some(const MutableBufferSequence &bs,
                                  error_code &error)
{
  native_buffer_sequence<MutableBufferSequence> nbs(bs);
  ssize_t nbytes = ::readv(fd_, nbs.iov.data(), nbs.iov.size());
  if (nbytes == -1)
  {
    error = {errno, system_category()};
  }
  else
  {
    error = {};
  }
  return nbytes;
}

} // namespace detail
} // namespace gpcl
