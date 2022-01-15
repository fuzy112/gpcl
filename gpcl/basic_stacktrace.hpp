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

#if defined(GPCL_WINDOWS)
#  include <gpcl/detail/win_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#elif defined(GPCL_POSIX)
#  include <gpcl/detail/posix_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#endif

#include <memory>

namespace gpcl {

#if defined(GPCL_DOXYGEN)

class stacktrace_entry
{
public:
  union native_handle_type;

  constexpr stacktrace_entry() noexcept = default;

  constexpr stacktrace_entry(const stacktrace_entry &) = default;

  constexpr stacktrace_entry &
  operator=(const stacktrace_entry &) = default;

  constexpr native_handle_type native_handle() const noexcept;

  constexpr explicit operator bool() const noexcept;

  GPCL_DECL std::string description() const;

  GPCL_DECL std::string source_file() const;

  GPCL_DECL std::uint_least32_t source_line() const;

  friend inline bool operator<(const stacktrace_entry &x,
                               const stacktrace_entry &y);

  friend inline bool operator!=(const stacktrace_entry &x,
                                const stacktrace_entry &y);

  friend inline bool operator==(const stacktrace_entry &x,
                                const stacktrace_entry &y);
  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os,
             const stacktrace_entry &f);
};

template <typename Allocator>
class basic_stacktrace
{
public:
  using allocator_type = Allocator;
  using value_type = stacktrace_entry;
  using size_type = std::size_t;
  struct const_iterator;
  using iterator = const_iterator;
  using reference = value_type &;
  using const_reference = value_type const &;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;

  basic_stacktrace();

  basic_stacktrace(const allocator_type &alloc) noexcept;

  basic_stacktrace(basic_stacktrace &&) noexcept;
  basic_stacktrace(basic_stacktrace const &);

  basic_stacktrace(basic_stacktrace &&, const allocator_type &) noexcept;
  basic_stacktrace(basic_stacktrace const &, const allocator_type &);

  basic_stacktrace &operator=(basic_stacktrace &&) noexcept;
  basic_stacktrace &operator=(basic_stacktrace const &);

  ~basic_stacktrace();

  static basic_stacktrace
  current(allocator_type const &alloc = allocator_type()) const noexcept;
  static basic_stacktrace
  current(size_type skip,
          allocator_type const &alloc = allocator_type()) const noexcept;
  static basic_stacktrace
  current(size_type skip, size_type max_depth,
          allocator_type const &alloc = allocator_type()) const noexcept;

  const_reference operator[](size_type pos) const noexcept;
  const_reference at(size_type pos);

  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cencd() const noexcept;

  reverse_const_iterator rbegin() const noexcept;
  reverse_const_iterator crbegin() const noexcept;
  reverse_const_iterator rend() const noexcept;
  reverse_const_iterator crend() const noexcept;

  size_type size() const noexcept;
  size_type max_size() const noexcept;

  allocator_type get_allocator() const noexcept;

  void swap(basic_stacktrace &other) const noexcept;
};

/// @relates gpcl::basic_stacktrace
template <typename Allocator>
void swap(basic_stacktrace<Allocator> &x,
          basic_stacktrace<Allocator> &y) noexcept(noexcept(x.swap(y)));

#elif defined(GPCL_WINDOWS)
template <typename Allocator>
using basic_stacktrace = detail::basic_win_stacktrace<Allocator>;
using stacktrace_entry = detail::win_stacktrace_entry;
#elif defined(GPCL_POSIX)
template <typename Allocator>
using basic_stacktrace = detail::basic_posix_stacktrace<Allocator>;
using stacktrace_entry = detail::posix_stacktrace_entry;
#endif

} // namespace gpcl

#endif // GPCL_BASIC_STACKTRACE_HPP
