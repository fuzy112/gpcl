//
// basic_spanstream.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BASIC_SPANSTREAM_HPP
#define GPCL_BASIC_SPANSTREAM_HPP

#include <gpcl/basic_spanbuf.hpp>

#include <iostream>

namespace gpcl {
namespace detail {

template <typename CharType, typename Traits>
class spanstream_base
{
protected:
  template <typename... Args>
  explicit spanstream_base(Args &&...args)
      : spanbuf_(std::forward<Args>(args)...)
  {
  }

  spanstream_base(spanstream_base &&) = default;

  spanstream_base &operator=(spanstream_base &&) = default;

  void swap(spanstream_base &other) { spanbuf_.swap(other.spanbuf_); }

  mutable basic_spanbuf<CharType, Traits> spanbuf_;
};
} // namespace detail

template <typename CharType, typename Traits = std::char_traits<CharType>>
class basic_ispanstream : private detail::spanstream_base<CharType, Traits>,
                          public std::basic_istream<CharType, Traits>
{
  using stream_type = std::basic_istream<CharType, Traits>;

public:
  /// @name Constructors
  /// @{
  explicit basic_ispanstream(gpcl::span<CharType> s,
                             std::ios_base::openmode mode = std::ios_base::in)
      : detail::spanstream_base<CharType, Traits>(s, mode),
        stream_type(rdbuf())
  {
  }

  basic_ispanstream(const basic_ispanstream &) = delete;

  basic_ispanstream(basic_ispanstream &&other)
      : detail::spanstream_base<CharType, Traits>(std::move(other)),
        stream_type(std::move(other))
  {
    this->set_rdbuf(rdbuf());
  }

  /// @}

  /// @name Assignment operators
  /// @{
  basic_ispanstream &operator=(const basic_ispanstream &) = delete;

  basic_ispanstream &operator=(basic_ispanstream &&other) = default;
  /// @}

  void swap(basic_ispanstream &other)
  {
    detail::spanstream_base<CharType, Traits>::swap(other);
    stream_type::swap(other);
  }

  basic_spanbuf<CharType, Traits> *rdbuf() const noexcept
  {
    return &this->spanbuf_;
  }

  gpcl::span<CharType> span() const noexcept { return rdbuf()->span(); }

  void span(gpcl::span<CharType> buf) noexcept { rdbuf()->span(buf); }
};

template <typename CharType, typename Traits>
void swap(basic_ispanstream<CharType, Traits> &x,
          basic_ispanstream<CharType, Traits> &y)
{
  x.swap(y);
}

using ispanstream = basic_ispanstream<char>;
using wispanstream = basic_ispanstream<wchar_t>;



template <typename CharType, typename Traits = std::char_traits<CharType>>
class basic_ospanstream : private detail::spanstream_base<CharType, Traits>,
                          public std::basic_istream<CharType, Traits>
{
  using stream_type = std::basic_istream<CharType, Traits>;

public:
  /// @name Constructors
  /// @{
  explicit basic_ospanstream(gpcl::span<CharType> s,
                             std::ios_base::openmode mode = std::ios_base::out)
      : detail::spanstream_base<CharType, Traits>(s, mode),
        stream_type(rdbuf())
  {
  }

  basic_ospanstream(const basic_ospanstream &) = delete;

  basic_ospanstream(basic_ospanstream &&other)
      : detail::spanstream_base<CharType, Traits>(std::move(other)),
        stream_type(std::move(other))
  {
    this->set_rdbuf(rdbuf());
  }

  /// @}

  /// @name Assignment operators
  /// @{
  basic_ospanstream &operator=(const basic_ospanstream &) = delete;

  basic_ospanstream &operator=(basic_ospanstream &&other) = default;
  /// @}

  void swap(basic_ospanstream &other)
  {
    detail::spanstream_base<CharType, Traits>::swap(other);
    stream_type::swap(other);
  }

  basic_spanbuf<CharType, Traits> *rdbuf() const noexcept
  {
    return &this->spanbuf_;
  }

  gpcl::span<CharType> span() const noexcept { return rdbuf()->span(); }

  void span(gpcl::span<CharType> buf) noexcept { rdbuf()->span(buf); }
};


template <typename CharType, typename Traits>
void swap(basic_ospanstream<CharType, Traits> &x,
          basic_ospanstream<CharType, Traits> &y)
{
  x.swap(y);
}

using ospanstream = basic_ospanstream<char>;
using wospanstream = basic_ospanstream<wchar_t>;

template <typename CharType, typename Traits = std::char_traits<CharType>>
class basic_spanstream : private detail::spanstream_base<CharType, Traits>,
                         public std::basic_iostream<CharType, Traits>
{
  using stream_type = std::basic_iostream<CharType, Traits>;

public:
  /// @name Constructors
  /// @{
  explicit basic_spanstream(gpcl::span<CharType> s,
                            std::ios_base::openmode mode = std::ios_base::in |
                                                           std::ios_base::out)
      : detail::spanstream_base<CharType, Traits>(s, mode),
        stream_type(rdbuf())
  {
  }

  basic_spanstream(const basic_spanstream &) = delete;

  basic_spanstream(basic_spanstream &&other)
      : detail::spanstream_base<CharType, Traits>(std::move(other)),
        stream_type(std::move(other))
  {
    this->set_rdbuf(rdbuf());
  }

  /// @}

  /// @name Assignment operators
  /// @{
  basic_spanstream &operator=(const basic_spanstream &) = delete;

  basic_spanstream &operator=(basic_spanstream &&other) = default;
  /// @}

  void swap(basic_spanstream &other)
  {
    detail::spanstream_base<CharType, Traits>::swap(other);
    stream_type::swap(other);
  }

  basic_spanbuf<CharType, Traits> *rdbuf() const noexcept
  {
    return &this->spanbuf_;
  }

  gpcl::span<CharType> span() const noexcept { return rdbuf()->span(); }

  void span(gpcl::span<CharType> buf) noexcept { rdbuf()->span(buf); }
};

template <typename CharType, typename Traits>
void swap(basic_spanstream<CharType, Traits> &x,
          basic_spanstream<CharType, Traits> &y)
{
  x.swap(y);
}

using spanstream = basic_spanstream<char>;
using wspanstream = basic_spanstream<wchar_t>;

} // namespace gpcl

#endif // GPCL_BASIC_SPANSTREAM_HPP
