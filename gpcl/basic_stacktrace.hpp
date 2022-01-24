//
// basic_stacktrace.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BASIC_STACKTRACE_HPP
#define GPCL_BASIC_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/swap.hpp>

#if defined(GPCL_NO_STACKTRACE)
#  define GPCL_STACKTRACE 0
#elif defined(GPCL_WINDOWS) || defined(__CYGWIN__)
#  include <gpcl/detail/win_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#elif defined(GPCL_POSIX) && !defined(__CYGWIN__)
#  include <gpcl/detail/posix_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#endif

#ifndef GPCL_STACKTRACE
#  define GPCL_NO_STACKTRACE
#  define GPCL_STACKTRACE 0
#endif

#ifdef GPCL_NO_STACKTRACE
#  include <gpcl/detail/error.hpp>

#  include <stdexcept>
#endif

#include <memory>
#include <string>

namespace gpcl {

#if defined(GPCL_NO_STACKTRACE) || defined(GPCL_DOXYGEN)

class stacktrace_entry
{
public:
  using native_handle_type = std::nullptr_t;

  constexpr stacktrace_entry() noexcept = default;

  constexpr stacktrace_entry(const stacktrace_entry &) = default;

  constexpr stacktrace_entry &operator=(const stacktrace_entry &) = default;

  constexpr native_handle_type native_handle() const noexcept
  {
    return nullptr;
  }

  constexpr explicit operator bool() const noexcept { return false; }

  std::string description() const { return ""; }

  std::string source_file() const { return ""; }

  std::uint_least32_t source_line() const { return 0; }

  friend inline bool operator<(const stacktrace_entry &,
                               const stacktrace_entry &)
  {
    return false;
  }

  friend inline bool operator!=(const stacktrace_entry &,
                                const stacktrace_entry &)
  {

    return false;
  }

  friend inline bool operator==(const stacktrace_entry &,
                                const stacktrace_entry &)
  {
    return true;
  }
  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os, const stacktrace_entry &)
  {
    return os;
  };
};

template <typename Allocator>
class basic_stacktrace : Allocator
{
public:
  using allocator_type = Allocator;
  using value_type = stacktrace_entry;
  using size_type = std::size_t;
  using const_iterator = const stacktrace_entry *;
  using iterator = const_iterator;
  using reference = value_type &;
  using const_reference = value_type const &;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;

  basic_stacktrace() = default;

  basic_stacktrace(const allocator_type &alloc) noexcept : Allocator(alloc) {}

  basic_stacktrace(basic_stacktrace &&) noexcept = default;
  basic_stacktrace(basic_stacktrace const &) = default;

  basic_stacktrace(basic_stacktrace &&, const allocator_type &a) noexcept
      : Allocator(a)
  {
  }
  basic_stacktrace(basic_stacktrace const &, const allocator_type &a)
      : Allocator(a)
  {
  }

  basic_stacktrace &operator=(basic_stacktrace &&) noexcept = default;
  basic_stacktrace &operator=(basic_stacktrace const &) = default;

  ~basic_stacktrace() = default;

  static basic_stacktrace
  current(allocator_type const &alloc = allocator_type()) noexcept
  {
    return basic_stacktrace(alloc);
  }
  static basic_stacktrace
  current(size_type skip,
          allocator_type const &alloc = allocator_type()) noexcept
  {
    (void)skip;
    return basic_stacktrace(alloc);
  }
  static basic_stacktrace
  current(size_type skip, size_type max_depth,
          allocator_type const &alloc = allocator_type()) noexcept
  {
    (void)skip;
    (void)max_depth;
    return basic_stacktrace(alloc);
  }

  const_reference operator[](size_type pos) const noexcept
  {
    (void)pos;
    std::terminate();
  }
  const_reference at(size_type pos) const
  {
    (void)pos;
    GPCL_THROW(std::out_of_range("stacktrace::at"));
  }

  const_iterator begin() const noexcept { return const_iterator(); }
  const_iterator cbegin() const noexcept { return const_iterator(); }
  const_iterator end() const noexcept { return const_iterator(); }
  const_iterator cencd() const noexcept { return const_iterator(); }

  reverse_const_iterator rbegin() const noexcept
  {
    return reverse_const_iterator();
  }
  reverse_const_iterator crbegin() const noexcept
  {
    return reverse_const_iterator();
  }
  reverse_const_iterator rend() const noexcept
  {
    return reverse_const_iterator();
  }
  reverse_const_iterator crend() const noexcept
  {
    return reverse_const_iterator();
  }

  size_type size() const noexcept { return 0; }
  size_type max_size() const noexcept { return 0; }

  allocator_type get_allocator() const noexcept { return *this; }

  void swap(basic_stacktrace &other) const noexcept
  {
    using gpcl::swap;
    swap(static_cast<Allocator &>(*this), static_cast<Allocator &>(other));
  }
};

/// @relates gpcl::basic_stacktrace
template <typename Allocator>
void swap(basic_stacktrace<Allocator> &x,
          basic_stacktrace<Allocator> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template <typename CharT, typename Traits, typename Allocator>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const basic_stacktrace<Allocator> &)
{
  return os;
};

#elif defined(GPCL_WINDOWS) || defined(__CYGWIN__)
template <typename Allocator>
using basic_stacktrace = detail::basic_win_stacktrace<Allocator>;
using stacktrace_entry = detail::win_stacktrace_entry;
#elif defined(GPCL_POSIX) && !defined(__CYGWIN__)
template <typename Allocator>
using basic_stacktrace = detail::basic_posix_stacktrace<Allocator>;
using stacktrace_entry = detail::posix_stacktrace_entry;
#endif

} // namespace gpcl

#endif // GPCL_BASIC_STACKTRACE_HPP
