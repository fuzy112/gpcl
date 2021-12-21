//
// buffer_sequence.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BUFFER_SEQUENCE_HPP
#define GPCL_BUFFER_SEQUENCE_HPP

#include <gpcl/buffer.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>

namespace gpcl {

namespace detail_buffer_sequence {
template <typename Buffer,
          std::enable_if_t<
              std::is_convertible<const Buffer *, const const_buffer *>::value,
              int> = 0>
const Buffer *buffer_sequence_begin(const Buffer &b) noexcept
{
  return std::addressof(b);
}

template <typename Container,
          std::enable_if_t<!std::is_convertible<const Container *,
                                                const const_buffer *>::value,
                           int> = 0>
auto buffer_sequence_begin(const Container &b) noexcept
{
  return std::begin(b);
}

template <typename Buffer,
          std::enable_if_t<
              std::is_convertible<const Buffer *, const const_buffer *>::value,
              int> = 0>
const Buffer *buffer_sequence_end(const Buffer &b) noexcept
{
  return std::addressof(b) + 1;
}

template <typename Container,
          std::enable_if_t<!std::is_convertible<const Container *,
                                                const const_buffer *>::value,
                           int> = 0>
auto buffer_sequence_end(const Container &b) noexcept
{
  return std::end(b);
}

} // namespace detail_buffer_sequence

struct buffer_sequence_begin_t
{
#ifndef GPCL_DOXYGEN
  template <typename BufferSequence>
  auto operator()(const BufferSequence &bs) const noexcept
  {
    using detail_buffer_sequence::buffer_sequence_begin;

    return buffer_sequence_begin(bs);
  }
#endif
};

struct buffer_sequence_end_t
{
#ifndef GPCL_DOXYGEN
  template <typename BufferSequence>
  auto operator()(const BufferSequence &bs) const noexcept
  {
    using detail_buffer_sequence::buffer_sequence_end;

    return buffer_sequence_end(bs);
  }
#endif
};

inline constexpr buffer_sequence_begin_t buffer_sequence_begin{};
inline constexpr buffer_sequence_end_t buffer_sequence_end{};

#ifndef GPCL_DOXYGEN
template <typename T, typename = void>
struct is_mutable_buffer_sequence : std::false_type
{
};

template <typename T>
struct is_mutable_buffer_sequence<
    T, std::enable_if_t<detail::conjunction_v<
           std::is_destructible<T>, std::is_copy_constructible<T>,
           std::is_convertible<decltype(*gpcl::buffer_sequence_begin(
                                   std::declval<T &>())),
                               mutable_buffer>,
           std::is_convertible<decltype(*gpcl::buffer_sequence_end(
                                   std::declval<T &>())),
                               mutable_buffer>>>> : std::true_type
{
};

template <typename T, typename = void>
struct is_const_buffer_sequence : std::false_type
{
};

template <typename T>
struct is_const_buffer_sequence<
    T, std::enable_if_t<detail::conjunction_v<
           std::is_destructible<T>, std::is_copy_constructible<T>,
           std::is_convertible<decltype(*gpcl::buffer_sequence_begin(
                                   std::declval<T &>())),
                               const_buffer>,
           std::is_convertible<decltype(*gpcl::buffer_sequence_end(
                                   std::declval<T &>())),
                               const_buffer>>>> : std::true_type
{
};
#endif

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

#endif // GPCL_BUFFER_SEQUENCE_HPP
