//
// span.hpp
// ~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SPAN_HPP
#define GPCL_SPAN_HPP

#include <gpcl/array.hpp>
#include <gpcl/assert.hpp>
#include <gpcl/default_allocator.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/detail/utility.hpp>

#include <array>

#if __cplusplus >= 201703
#  include <string_view>
#endif

namespace gpcl {

namespace detail {
template <typename T>
class dynamic_span_base
{
  T *p_{};
  std::size_t s_{};

public:
  constexpr dynamic_span_base() = default;
  constexpr dynamic_span_base(const dynamic_span_base &) = default;
  constexpr dynamic_span_base &operator=(const dynamic_span_base &) = default;

  constexpr dynamic_span_base(T *p, std::size_t s) noexcept : p_(p), s_(s) {}

  [[nodiscard]] constexpr T *data_() const noexcept { return p_; }
  [[nodiscard]] constexpr std::size_t size_() const noexcept { return s_; }
};

template <typename T, std::size_t S>
class static_span_base
{
  T *p_{};

public:
  static const std::size_t extent = S;

public:
  template <bool C = S == 0, std::enable_if_t<C, int> = 0>
  constexpr static_span_base()
  {
  }

  constexpr static_span_base(const static_span_base &) = default;
  constexpr static_span_base &operator=(const static_span_base &) = default;

  constexpr static_span_base(T *p, std::size_t s) /* noexcept */
      : p_(p)
  {
    GPCL_ASSERT(s == extent);
    (void)s;
  }

  [[nodiscard]] constexpr T *data_() const noexcept { return p_; }
  [[nodiscard]] constexpr std::size_t size_() const noexcept { return extent; }
};

template <typename T, std::size_t E>
using span_base =
    std::conditional_t<E == std::size_t(-1), detail::dynamic_span_base<T>,
                       detail::static_span_base<T, E>>;

} // namespace detail

/// A contiguous container view.
template <typename T, std::size_t E = dynamic_extent>
class span;

template <typename T, std::size_t E>
class span : detail::span_base<T, E>
{
  using base_type = detail::span_base<T, E>;

public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using iterator = T *;
  using reverse_iterator = std::reverse_iterator<iterator>;

  constexpr span() noexcept = default;

  template <typename It> // requires ContiguousIterator<It>
  inline constexpr span(It first, size_type count)
      : base_type(std::addressof(*first), count)
  {
  }

  template <
      typename It, typename End,
      std::enable_if_t<detail::negate_v<std::is_convertible<End, std::size_t>>,
                       int>
          Dummy = 0> // requires ContiguousIterator<It>
  inline constexpr span(It first, End last)
      : base_type(std::addressof(*first),
                  std::addressof(*last) - std::addressof(*first))
  {
  }

  template <std::size_t N,
            std::enable_if_t<N == E || E == dynamic_extent, int> = 0>
  inline constexpr span(element_type (&arr)[N]) noexcept : base_type(arr, N)
  {
  }

  template <typename U, std::size_t N,
            std::enable_if_t<N == E || E == dynamic_extent, int> = 0>
  inline constexpr span(std::array<U, N> &arr) noexcept
      : base_type(arr.data(), N)
  {
  }

  template <typename U, std::size_t N,
            std::enable_if_t<N == E || E == dynamic_extent, int> = 0>
  inline constexpr span(const std::array<U, N> &arr) noexcept
      : base_type(arr.data(), N)
  {
  }

  template <typename R>
  inline constexpr span(R &&r) : base_type(r.data(), r.size())
  {
  }

  template <typename U, std::size_t N,
            std::enable_if_t<
                N == E || E == dynamic_extent || E == dynamic_extent, int> = 0>
  inline constexpr span(const span<U, N> &s) noexcept
      : base_type(s.data(), s.size())
  {
  }

  constexpr span(const span &other) noexcept = default;

  constexpr span &operator=(const span &other) noexcept = default;

  [[nodiscard]] inline constexpr iterator begin() const noexcept
  {
    return iterator{data()};
  }

  [[nodiscard]] inline constexpr iterator end() const noexcept
  {
    return begin() + size();
  }

  [[nodiscard]] inline constexpr iterator rbegin() const noexcept
  {
    return std::make_reverse_iterator(end());
  }

  [[nodiscard]] inline constexpr iterator rend() const noexcept
  {
    return std::make_reverse_iterator(begin());
  }

  [[nodiscard]] inline constexpr reference front() const { return *begin(); }

  [[nodiscard]] inline constexpr reference back() const { return *rbegin(); }

  [[nodiscard]] inline constexpr reference operator[](size_type idx) const
  {
    return data()[idx];
  }

  [[nodiscard]] inline constexpr pointer data() const noexcept
  {
    return this->data_();
  }

  [[nodiscard]] inline constexpr size_type size() const noexcept
  {
    return this->size_();
  }

  [[nodiscard]] inline constexpr size_type size_bytes() const noexcept
  {
    return size() * sizeof(element_type);
  }

  [[nodiscard]] inline constexpr bool empty() const { return size() == 0; }

  [[nodiscard]] inline constexpr span<T> first(size_type count) const
  {
    GPCL_ASSERT(count <= size());
    return span(data(), count);
  }

  template <std::size_t N>
  [[nodiscard]] inline constexpr span<T, N> first() const
  {
    static_assert(N <= E);
    return span<T, N>(data(), N);
  }

  [[nodiscard]] inline constexpr span<T> last(size_type count) const
  {
    GPCL_ASSERT(count <= size());
    return span(end() - count, end());
  }

  template <std::size_t N>
  [[nodiscard]] inline constexpr span<T, N> last() const
  {
    static_assert(N <= E);
    return span<T, N>(end() - N, end());
  }

  [[nodiscard]] inline constexpr span<T>
  subspan(size_type offset, size_type count = dynamic_extent) const
  {
    GPCL_ASSERT(offset <= size());

    if (count == dynamic_extent)
    {
      return span(begin() + offset, end());
    }

    GPCL_ASSERT(offset + count <= size());
    return span(begin() + offset, begin() + offset + count);
  }

  template <
      std::size_t Offset, std::size_t Count = dynamic_extent,
      std::size_t Extent = std::conditional_t<
          (Count != dynamic_extent), std::integral_constant<std::size_t, Count>,
          std::conditional_t<
              E != dynamic_extent,
              std::integral_constant<std::size_t, E - Offset>,
              std::integral_constant<std::size_t, dynamic_extent>>>::value>
  [[nodiscard]] inline constexpr span<T, Extent> subspan() const
  {
    return span<T, Extent>(data() + Offset,
                           Count != dynamic_extent ? Count : size() - Offset);
  }

  // compares

  template <typename U = T, std::size_t N>
  inline constexpr bool operator==(span<U, N> other) const
  {
    static_assert(std::is_same<std::decay_t<U>, value_type>{}, "");
    return std::equal(begin(), end(), other.begin(), other.end());
  }

  template <typename U = T, std::size_t N>
  inline constexpr bool operator!=(span<U, N> other) const
  {
    return !(*this == other);
  }

  template <typename U = T, std::size_t N>
  inline constexpr bool operator<(span<U, N> other) const
  {
    static_assert(std::is_same<std::decay_t<U>, value_type>{}, "");
    return std::lexicographical_compare(begin(), end(), other.begin(),
                                        other.end());
  }

  template <typename U = T, std::size_t N>
  inline constexpr bool operator>(span<U, N> other) const
  {
    return other < *this;
  }

  template <typename U = T, std::size_t N>
  inline constexpr bool operator<=(span<U, N> other) const
  {
    return !(other < *this);
  }

  template <typename U = T, std::size_t N>
  inline constexpr bool operator>=(span<U, N> other) const
  {
    return !(*this < other);
  }

  // extensions

#if __cplusplus >= 201703L
  template <typename U = value_type,
            std::enable_if_t<detail::is_char_like_type_v<U>, int> = 0>
  [[nodiscard]] std::basic_string_view<U> to_string_view() const
  {
    return std::basic_string_view<U>(data(), size());
  }
#endif
};

#if __cplusplus >= 201703 && !defined(GPCL_STANDARDESE)

template <typename T, std::size_t N>
span(T (&)[N]) -> span<T, N>;

template <typename T, std::size_t N>
span(std::array<T, N> &) -> span<T, N>;

template <typename It, typename EndOrSize>
span(It, EndOrSize) -> span<typename std::iterator_traits<It>::reference>;

template <typename T, std::size_t N>
span(const std::array<T, N> &) -> span<const T, N>;

#endif

template <typename T, std::size_t N>
[[nodiscard]] inline auto as_bytes(span<T, N> s) noexcept
{
  return span<const unsigned char,
              (N == dynamic_extent ? dynamic_extent : N * sizeof(T))>(
      reinterpret_cast<const unsigned char *>(s.data()), s.size_bytes());
}

template <typename T, std::size_t N>
[[nodiscard]] inline auto as_writable_bytes(span<T, N> s) noexcept
{
  return span<unsigned char,
              (N == dynamic_extent ? dynamic_extent : N * sizeof(T))>(
      reinterpret_cast<unsigned char *>(s.data()), s.size_bytes());
}

} // namespace gpcl

#endif // GPCL_SPAN_HPP
