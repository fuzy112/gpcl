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

struct buffer_sequence_begin_t
{

  template <typename MutableBuffer,
            std::enable_if_t<std::is_convertible<const MutableBuffer *,
                                                 const mutable_buffer *>::value,
                             int> = 0>
  const mutable_buffer *operator()(const MutableBuffer &b) const
  {
    return std::addressof(b);
  }

  template <typename ConstBuffer,
            std::enable_if_t<std::is_convertible<const ConstBuffer *,
                                                 const const_buffer *>::value,
                             int> = 0>
  const const_buffer *operator()(const ConstBuffer &b) const
  {
    return std::addressof(b);
  }

  template <typename C,
            std::enable_if_t<
                detail::conjunction_v<
                    detail::negate<
                        std::is_convertible<const C *, const mutable_buffer *>>,
                    detail::negate<
                        std::is_convertible<const C *, const const_buffer *>>,
                    std::is_void<detail::void_t<
                        decltype(buffer_sequence_begin(std::declval<C &>()))>>>,
                int> = 0>
  auto operator()(C &c) const
  {
    return buffer_sequence_begin(c);
  }

  template <typename C,
            std::enable_if_t<
                detail::conjunction_v<
                    detail::negate<
                        std::is_convertible<const C *, const mutable_buffer *>>,
                    detail::negate<
                        std::is_convertible<const C *, const const_buffer *>>,
                    detail::negate<std::is_void<detail::void_t<decltype(
                        buffer_sequence_begin(std::declval<C &>()))>>>>,
                int> = 0>
  auto operator()(C &c) const
  {
    return std::begin(c);
  }

  template <typename C,
            std::enable_if_t<
                detail::conjunction_v<
                    detail::negate<
                        std::is_convertible<const C *, const mutable_buffer *>>,
                    detail::negate<
                        std::is_convertible<const C *, const const_buffer *>>,
                    std::is_void<detail::void_t<
                        decltype(buffer_sequence_begin(std::declval<C &>()))>>>,
                int> = 0>
  auto operator()(const C &c) const
  {
    return buffer_sequence_begin(c);
  }

  template <typename C,
            std::enable_if_t<
                detail::conjunction_v<
                    detail::negate<
                        std::is_convertible<const C *, const mutable_buffer *>>,
                    detail::negate<
                        std::is_convertible<const C *, const const_buffer *>>,
                    detail::negate<std::is_void<detail::void_t<decltype(
                        buffer_sequence_begin(std::declval<C &>()))>>>>,
                int> = 0>
  auto operator()(const C &c) const
  {
    return std::begin(c);
  }
};

struct buffer_sequence_end_t
{
  template <typename MutableBuffer,
            std::enable_if_t<std::is_convertible<const MutableBuffer *,
                                                 const mutable_buffer *>::value,
                             int> = 0>
  const mutable_buffer *operator()(const MutableBuffer &b) const
  {
    return std::addressof(b) + 1;
  }

  template <typename ConstBuffer,
            std::enable_if_t<std::is_convertible<const ConstBuffer *,
                                                 const const_buffer *>::value,
                             int> = 0>
  const const_buffer *operator()(const ConstBuffer &b) const
  {
    return std::addressof(b) + 1;
  }

  template <typename C,
            std::enable_if_t<
                detail::conjunction_v<
                    detail::negate<
                        std::is_convertible<const C *, const mutable_buffer *>>,
                    detail::negate<
                        std::is_convertible<const C *, const const_buffer *>>,
                    std::is_void<detail::void_t<
                        decltype(buffer_sequence_end(std::declval<C &>()))>>>,
                int> = 0>
  auto operator()(const C &c) const
  {
    return buffer_sequence_end(c);
  }

  template <typename C,
            std::enable_if_t<
                detail::conjunction_v<
                    detail::negate<
                        std::is_convertible<const C *, const mutable_buffer *>>,
                    detail::negate<
                        std::is_convertible<const C *, const const_buffer *>>,
                    detail::negate<std::is_void<detail::void_t<
                        decltype(buffer_sequence_end(std::declval<C &>()))>>>>,
                int> = 0>
  auto operator()(const C &c) const
  {
    return std::end(c);
  }
};

GPCL_CXX17_INLINE_CONSTEXPR buffer_sequence_begin_t buffer_sequence_begin{};
GPCL_CXX17_INLINE_CONSTEXPR buffer_sequence_end_t buffer_sequence_end{};

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

} // namespace gpcl

#endif // GPCL_BUFFER_SEQUENCE_HPP
