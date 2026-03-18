//
// posix_file.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
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

#if defined(GPCL_LINUX) || defined(__FreeBSD__ ) || defined(__EMSCRIPTEN__)
template <typename ConstBufferSequence>
expected<std::size_t, error_code>
posix_file::write_some_at(offset_type off, const ConstBufferSequence &bs)
{
  native_buffer_sequence<ConstBufferSequence> nbs(bs);
  ssize_t nbytes = ::pwritev(fd_, nbs.iov.data(), nbs.iov.size(), off);
  if (nbytes == -1)
  {
    return unexpected<error_code>(in_place, errno, system_category());
  }
  return nbytes;
}
#endif

#if defined(GPCL_LINUX) || defined(__FreeBSD__ ) || defined(__EMSCRIPTEN__)
template <typename MutableBufferSequence>
expected<size_t, error_code>
posix_file::read_some_at(offset_type off, const MutableBufferSequence &bs)
{
  native_buffer_sequence<MutableBufferSequence> nbs(bs);
  ssize_t nbytes = ::preadv(fd_, nbs.iov.data(), nbs.iov.size(), off);
  if (nbytes == -1)
  {
    return unexpected<error_code>{in_place, errno, system_category()};
  }
  return nbytes;
}
#endif

template <typename ConstBufferSequence>
expected<std::size_t, error_code>
posix_file::write_some(const ConstBufferSequence &bs)
{
  native_buffer_sequence<ConstBufferSequence> nbs(bs);
  ssize_t nbytes = ::writev(fd_, nbs.iov.data(), nbs.iov.size());
  if (nbytes == -1)
  {
    return unexpected<error_code>{in_place, errno, system_category()};
  }
  return nbytes;
}

template <typename MutableBufferSequence>
expected<std::size_t, error_code>
posix_file::read_some(const MutableBufferSequence &bs)
{
  native_buffer_sequence<MutableBufferSequence> nbs(bs);
  ssize_t nbytes = ::readv(fd_, nbs.iov.data(), nbs.iov.size());
  if (nbytes == -1)
  {
    return unexpected<error_code>{in_place, errno, system_category()};
  }
  return nbytes;
}

} // namespace detail
} // namespace gpcl
