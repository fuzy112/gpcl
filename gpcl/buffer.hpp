//
// buffer.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BUFFER_HPP
#define GPCL_BUFFER_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/span.hpp>

namespace gpcl {

using const_buffer = span<const char>;
using mutable_buffer = span<char>;

/// Consume some bytes of the buffer.
inline const_buffer &operator+=(const_buffer &buf, std::size_t sz)
{
  return buf = buf.subspan(sz);
}

/// Consume some bytes of the buffer.
inline mutable_buffer &operator+=(mutable_buffer &buf, std::size_t sz)
{
  return buf = buf.subspan(sz);
}

/// Create a buffer from raw memory.
inline const_buffer buffer(const void *data, std::size_t size)
{
  return const_buffer(reinterpret_cast<const char *>(data), size);
}

/// Create a buffer from raw memory.
inline mutable_buffer buffer(void *data, std::size_t size)
{
  return mutable_buffer(reinterpret_cast<char *>(data), size);
}

/// Make a buffer from a contiguous container.
template <typename T>
mutable_buffer
buffer(T &obj, detail::void_t<decltype(std::declval<T &>().data(),
                                       std::declval<T &>().size())> * = nullptr)
{
  return buffer(obj.data(), obj.size() * sizeof(*obj.data()));
}

/// Make a buffer from a contiguous container.
template <typename T>
const_buffer
buffer(const T &obj,
       detail::void_t<decltype(std::declval<const T &>().data(),
                               std::declval<const T &>().size())> * = nullptr)
{
  return buffer(obj.data(), obj.size() * sizeof(*obj.data()));
}

} // namespace gpcl

#endif // GPCL_BUFFER_HPP
