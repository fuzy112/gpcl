//
// buffer_iterator.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BUFFERS_ITERATOR_HPP
#define GPCL_BUFFERS_ITERATOR_HPP

#include <gpcl/buffer.hpp>
#include <gpcl/buffer_sequence.hpp>

#include <iterator>
#include <type_traits>

namespace gpcl {
template <typename BufferSequence>
struct buffers_iterator
{
  typedef std::forward_iterator_tag iterator_category;

  using buffer_sequence_iterator = std::decay_t<decltype(
      buffer_sequence_begin(std::declval<BufferSequence>()))>;

  buffer_sequence_iterator curr_buf;
  std::size_t byte_pos;

  buffers_iterator(buffer_sequence_iterator curr_buf,
                   std::size_t byte_pos) noexcept
      : curr_buf(curr_buf),
        byte_pos(byte_pos)
  {
  }

  decltype(auto) operator*() const { return *operator->(); }

  auto operator->() const { return curr_buf->begin() + byte_pos; }

  /// @bug this does not work when a buffer is empty.
  buffers_iterator &operator++()
  {
    ++byte_pos;
    if (byte_pos >= curr_buf->size())
    {
      ++curr_buf;
      byte_pos = 0;
    }
    return *this;
  };

  buffers_iterator operator++(int)
  {
    auto r = *this;
    ++*this;
    return r;
  }

  bool operator==(const buffers_iterator &other) const noexcept
  {
    return curr_buf == other.curr_buf && byte_pos == other.byte_pos;
  }

  bool operator!=(const buffers_iterator &other) const noexcept
  {
    return !(*this == other);
  }
};

template <typename BufferSequence>
buffers_iterator<BufferSequence> buffers_begin(BufferSequence &&bs) noexcept
{
  return buffers_iterator<BufferSequence>(buffer_sequence_begin(bs), 0);
}

template <typename BufferSequence>
buffers_iterator<BufferSequence> buffers_end(BufferSequence &&bs) noexcept
{
  return buffers_iterator<BufferSequence>(buffer_sequence_end(bs), 0);
}

inline auto buffers_begin(const_buffer b) noexcept
{
  return b.begin();
}

inline auto buffers_end(const_buffer b) noexcept
{
  return b.end();
}

inline auto buffers_begin(mutable_buffer b) noexcept
{
  return b.begin();
}

inline auto buffers_end(mutable_buffer b) noexcept
{
  return b.end();
}

} // namespace gpcl

#endif // GPCL_BUFFERS_ITERATOR_HPP
