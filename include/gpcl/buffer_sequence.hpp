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

#include <gpcl/const_buffer.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/mutable_buffer.hpp>
#include <gpcl/tag_invoke.hpp>

#include <iterator>

namespace gpcl {

namespace detail {

struct buffer_sequence_begin_fn
{
  template <
      typename T,
      typename R = tag_invoke_result_t<buffer_sequence_begin_fn, const T &>,
      std::enable_if_t<
          std::is_convertible_v<typename std::iterator_traits<R>::value_type,
                                const_buffer>,
          int> = 0>
  decltype(auto) operator()(const T &x) const
  {
    return gpcl::tag_invoke(*this, x);
  }
};

struct buffer_sequence_end_fn
{
  template <typename T,
            typename R = tag_invoke_result_t<buffer_sequence_end_fn, const T &>,
            std::enable_if_t<
                std::is_convertible_v<
                    typename std::iterator_traits<R>::value_type, const_buffer>,
                int> = 0>
  decltype(auto) operator()(const T &x) const
  {
    return gpcl::tag_invoke(*this, x);
  }
};

template <typename Buffer,
          std::enable_if_t<
              std::is_convertible<const Buffer *, const const_buffer *>::value,
              int> = 0>
const Buffer *buffer_sequence_begin(const Buffer &b) noexcept
{
  return std::addressof(b);
}

template <
    typename Container,
    std::enable_if_t<
        !std::is_convertible<const Container *, const const_buffer *>::value &&
            std::is_convertible_v<
                typename std::iterator_traits<decltype(std::begin(
                    std::declval<Container const &>()))>::value_type,
                const_buffer>,
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

template <
    typename Container,
    std::enable_if_t<
        !std::is_convertible<const Container *, const const_buffer *>::value &&
            std::is_convertible_v<
                typename std::iterator_traits<decltype(std::end(
                    std::declval<Container const &>()))>::value_type,
                const_buffer>,
        int> = 0>
auto buffer_sequence_end(const Container &b) noexcept
{
  return std::end(b);
}

template <
    typename T,
    typename R = decltype(buffer_sequence_begin(std::declval<const T &>())),
    std::enable_if_t<
        std::is_convertible_v<typename std::iterator_traits<R>::value_type,
                              const_buffer>,
        int> = 0>
decltype(auto) tag_invoke(buffer_sequence_begin_fn, const T &x)
{
  return buffer_sequence_begin(x);
}

template <typename T,
          typename R = decltype(buffer_sequence_end(std::declval<const T &>())),
          std::enable_if_t<
              std::is_convertible_v<
                  typename std::iterator_traits<R>::value_type, const_buffer>,
              int> = 0>
decltype(auto) tag_invoke(buffer_sequence_end_fn, const T &x)
{
  return buffer_sequence_end(x);
}

} // namespace detail

using buffer_sequence_begin_fn = detail::buffer_sequence_begin_fn;
using buffer_sequence_end_fn = detail::buffer_sequence_end_fn;

/// Returns iterator to the first buffer of the buffer sequence.
/**
 * @ingroup customisation_point
 */
inline constexpr buffer_sequence_begin_fn buffer_sequence_begin{};

/// Returns the past-the-end iterator of the buffer sequence.
/**
 * @ingroup customisation_point
 */
inline constexpr buffer_sequence_end_fn buffer_sequence_end{};

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

} // namespace gpcl

#endif // GPCL_BUFFER_SEQUENCE_HPP
