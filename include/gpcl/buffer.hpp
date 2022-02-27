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

#include <gpcl/const_buffer.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/mutable_buffer.hpp>

namespace gpcl {

namespace detail {

// Create a buffer from raw memory.
inline const_buffer buffer(const void *data, std::size_t size) noexcept
{
  return const_buffer(data, size);
}

// Create a buffer from raw memory.
inline mutable_buffer buffer(void *data, std::size_t size) noexcept
{
  return mutable_buffer(data, size);
}

// Make a buffer from a contiguous container.
template <typename T>
mutable_buffer buffer(
    T &obj,
    detail::void_t<decltype(std::declval<T &>().data(),
                            std::declval<T &>().size())> * = nullptr) noexcept
{
  return buffer(obj.data(), obj.size() * sizeof(*obj.data()));
}

// Make a buffer from a contiguous container.
template <typename T>
const_buffer
buffer(const T &obj,
       detail::void_t<decltype(std::declval<const T &>().data(),
                               std::declval<const T &>().size())> * =
           nullptr) noexcept
{
  return buffer(obj.data(), obj.size() * sizeof(*obj.data()));
}

struct buffer_fn
{
  template <typename... Args>
  auto operator()(Args &&... args) const
  {
    return buffer(static_cast<Args &&>(args)...);
  }
};

} // namespace detail

using buffer_fn = detail::buffer_fn;

/// Factory for mutable_buffer and constant_buffer.
/** Create a new mutable_buffer or constant_buffer.
 *
 *  @ingroup customisation_point
 *
 *  @par customisation Point
 *  This object is a customisation point object.
 *
 *  @par Example
 *  @code {.cpp}
 *  char storage[4096];
 *  auto b = buffer(storage);
 *  @endcode
 */
inline constexpr buffer_fn buffer{};

} // namespace gpcl

#endif // GPCL_BUFFER_HPP
