//
// iovec.hpp
// ~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IOVEC_HPP
#define GPCL_DETAIL_IOVEC_HPP

#include <gpcl/buffer_sequence.hpp>
#include <gpcl/default_allocator.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/vector.hpp>

#include <cstdlib>
#include <sys/uio.h>

namespace gpcl {
namespace detail {

#ifdef GPCL_DOXYGEN
template <typename BufferSequence,
          typename Allocator = gpcl::default_allocator<char>>
struct native_buffer_sequence
{
  /// Constructor.
  explicit native_buffer_sequence(const BufferSequence &bs,
                                  const Allocator &a = Allocator());

  /// A contiguous sequence of struct ::iovec.
  unspecified - sequence iov;
};
#else

template <typename BufferSequence,
          typename Allocator = gpcl::default_allocator<char>>
struct native_buffer_sequence
{
  explicit native_buffer_sequence(const BufferSequence &bs,
                                  const Allocator &a = Allocator())
      : iov(a)
  {
    auto iter = buffer_sequence_begin(bs);
    const auto end = buffer_sequence_end(bs);
    iov.reserve(std::distance(iter, end));
    for (; iter != end; ++iter)
    {
      struct iovec io;
      io.iov_base = const_cast<void *>(static_cast<void *>(iter->data()));
      io.iov_len = iter->size();
      iov.emplace_back(io);
    }
  }

  gpcl::vector<struct ::iovec, typename std::allocator_traits<Allocator>::
                                   template rebind_alloc<struct ::iovec>>
      iov;
};

template <typename Buffer, std::size_t S, typename Allocator>
struct native_buffer_sequence<Buffer[S], Allocator>
{
  explicit native_buffer_sequence(const Buffer (&bs)[S])
  {
    auto io = iov.begin();
    for (auto iter = buffer_sequence_begin(bs), end = buffer_sequence_end(bs);
         iter != end; ++iter)
    {
      io->iov_base =
          const_cast<void *>(static_cast<const void *>(iter->data()));
      io->iov_len = iter->size();
      ++io;
    }
  }

  native_buffer_sequence(const Buffer (&bs)[S], const Allocator &)
      : native_buffer_sequence(bs)
  {
  }

  std::array<struct ::iovec, S> iov;
};

template <typename Buffer, std::size_t S, typename Allocator>
struct native_buffer_sequence<std::array<Buffer, S>, Allocator>
{
  explicit native_buffer_sequence(std::array<Buffer, S> const &bs) noexcept
  {
    auto io = iov.begin();
    for (auto iter = buffer_sequence_begin(bs), end = buffer_sequence_end(bs);
         iter != end; ++iter)
    {
      io->iov_base =
          const_cast<void *>(static_cast<const void *>(iter->data()));
      io->iov_len = iter->size();
      ++io;
    }
  }

  native_buffer_sequence(const std::array<Buffer, S> &bs,
                         const Allocator &) noexcept
      : native_buffer_sequence(bs)
  {
  }

  std::array<struct ::iovec, S> iov;
};

template <typename Allocator>
struct native_buffer_sequence<const_buffer, Allocator>
{
  explicit native_buffer_sequence(const_buffer b) noexcept
  {
    iov[0].iov_base = const_cast<unsigned char *>(b.data());
    iov[0].iov_len = b.size();
  }

  native_buffer_sequence(const_buffer bs, const Allocator &) noexcept
      : native_buffer_sequence(bs)
  {
  }

  std::array<struct ::iovec, 1> iov;
};

template <typename Allocator>
struct native_buffer_sequence<mutable_buffer, Allocator>
{
  explicit native_buffer_sequence(mutable_buffer b) noexcept
  {
    iov[0].iov_base = b.data();
    iov[0].iov_len = b.size();
  }

  native_buffer_sequence(mutable_buffer bs, const Allocator &) noexcept
      : native_buffer_sequence(bs)
  {
  }

  std::array<struct ::iovec, 1> iov;
};

#endif

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IOVEC_HPP
