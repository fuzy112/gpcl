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

#include <gpcl/buffer_sequence.hpp>
#include <gpcl/tag_invoke.hpp>

namespace gpcl {

namespace detail {

struct buffer_fn
{
  template <typename... Args>
  auto operator()(Args &&...args) const
      -> std::enable_if_t<is_const_buffer_sequence<tag_invoke_result_t<
                              buffer_fn, Args &&...>>::value,
                          tag_invoke_result_t<buffer_fn, Args &&...>>
  {
    return gpcl::tag_invoke(*this, static_cast<Args &&>(args)...);
  }
};

// Create a buffer from raw memory.
inline const_buffer buffer(const void *data, std::size_t size) noexcept
{
  return const_buffer(reinterpret_cast<const unsigned char *>(data), size);
}

// Create a buffer from raw memory.
inline mutable_buffer buffer(void *data, std::size_t size) noexcept
{
  return mutable_buffer(reinterpret_cast<unsigned char *>(data), size);
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

template <typename... Args>
auto tag_invoke(buffer_fn, Args &&...args)
    -> std::enable_if_t<is_const_buffer_sequence<decltype(buffer(
                            static_cast<Args &&>(args)...))>::value,
                        decltype(buffer(static_cast<Args &&>(args)...))>
{
  return buffer(static_cast<Args &&>(args)...);
}

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
