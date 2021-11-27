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

#include "gpcl/buffer_sequence.hpp"
#include "gpcl/detail/config.hpp"
#include "gpcl/vector.hpp"

#include <cstdlib>
#include <sys/uio.h>

namespace gpcl {
namespace detail {

template <std::size_t S>
struct static_iovec
{
  template <typename BufferSequence>
  explicit static_iovec(const BufferSequence &bs)
  {
    auto io = vec.begin();
    for (auto iter = buffer_sequence_begin(bs), end = buffer_sequence_end(bs);
         iter != end; ++iter)
    {
      io->iov_base = const_cast<void *>(static_cast<void *>(iter->data()));
      io->iov_len = iter->size();
      ++io;
    }
  }

  const struct iovec *iov() const { return vec; }

  constexpr static int iovcnt() { return S; }

  std::array<struct iovec, S> vec;
};

template <typename Allocator = std::allocator<void>>
struct dynamic_iovec
{
  template <typename BufferSequence>
  explicit dynamic_iovec(const BufferSequence &bs,
                         const Allocator &alloc = Allocator())
      : vec(alloc)
  {
    vec.resize(std::size(bs));
    auto io = vec.begin();
    for (auto iter = buffer_sequence_begin(bs), end = buffer_sequence_end(bs);
         iter != end; ++iter)
    {
      io->iov_base = const_cast<void *>(static_cast<void *>(iter->data()));
      io->iov_len = iter->size();
      ++io;
    }
  }

  const struct iovec *iov() const { return vec.data(); }

  int iovcnt() const { return vec.size(); }

  gpcl::vector<struct iovec, typename std::allocator_traits<
                                Allocator>::template rebind_alloc<struct iovec>>
      vec;
};

/// Concept IoVec
/** \par Example
 *  \code {c++}
 *  class example_iovec
 *  {
 *  public:
 *     const struct ::iovec *iov() const();
 * 
 *     int iovcnt() const;
 *  };
 *  \endcode 
 */

/// Create an IoVec for a buffer sequence.
template <typename BufferSequence>
auto make_iovec(const BufferSequence &bs)
{
  return dynamic_iovec(bs);
}

/// Create an IoVec for a buffer sequence.
template <typename Buffer, std::size_t S>
auto make_iovec(const std::array<Buffer, S> &bs)
{
  return static_iovec<S>(bs);
}

/// Create an IoVec for a buffer sequence.
template <typename Buffer, std::size_t S>
auto make_iovec(const Buffer (&bs)[S])
{
  return static_iovec<S>(bs);
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IOVEC_HPP
