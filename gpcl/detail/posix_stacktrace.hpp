//
// posix_stacktrace.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_STACKTRACE_HPP
#define GPCL_DETAIL_POSIX_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/unique_ptr.hpp>

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

#include <execinfo.h>

#define GPCL_BFD 1

#ifdef __GNUC__
#  include <cxxabi.h>
#endif

#ifdef GPCL_BFD
#  include <gpcl/detail/bfd_stacktrace.hpp>
#endif

namespace gpcl::detail {

#ifdef __GNUC__
inline std::string gcc_demangle(const char *sym)
{
  std::string name;
  size_t length = 0;
  int status = 0;
  __cxxabiv1::__cxa_demangle(sym, nullptr, &length, &status);

  name.resize(length);
  __cxxabiv1::__cxa_demangle(sym, &name[0], &length, &status);
  return name;
}

/// @todo refactor this function.
inline std::string cppfilt(std::string_view str)
{
  auto start = str.find("_Z");
  auto end = str.find("+");

  if (start == str.npos || end == str.npos)
  {
    return std::string(str);
  }

  /// @todo optimize memory allocation
  return std::string(str.substr(0, start)) +
         gcc_demangle(std::string(str.substr(start, end - start)).c_str()) +
         std::string(str.substr(end));
}

#else

inline std::string cppfilt(std::string_view str)
{
  return std::string(str);
}

#endif

struct posix_stacktrace_entry
{
  using native_handle_type = void *;

  void *address = nullptr;

  constexpr posix_stacktrace_entry() noexcept = default;

  constexpr posix_stacktrace_entry(const posix_stacktrace_entry &) noexcept =
      default;

  constexpr posix_stacktrace_entry &
  operator=(const posix_stacktrace_entry &) noexcept = default;

  std::string description() const
  {
#ifndef GPCL_BFD
    unique_ptr<char *, std::decay_t<decltype(::free)>> strs(
        ::backtrace_symbols(&address, 1), &::free);
    if (!strs)
      return "";
    if (!*strs)
      return "";
    return cppfilt(*strs);
#else
    return bfd_stacktrace_entry_description(address);
#endif
  }

  std::string source_file() const;
  std::uint_least32_t source_line() const;

  constexpr native_handle_type native_handle() const noexcept
  {
    return address;
  }

  constexpr explicit operator bool() const noexcept { return !!address; }

  friend inline bool operator<(const posix_stacktrace_entry &x,
                               const posix_stacktrace_entry &y)
  {
    return x.address < y.address;
  }

  friend inline bool operator!=(const posix_stacktrace_entry &x,
                                const posix_stacktrace_entry &y)
  {
    return x.address != y.address;
  }

  friend inline bool operator==(const posix_stacktrace_entry &x,
                                const posix_stacktrace_entry &y)
  {
    return x.address == y.address;
  }

  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os,
             const posix_stacktrace_entry &entry)
  {
    return os << entry.description();
  }
};

template <typename Allocator>
class basic_posix_stacktrace
{
  static_assert(
      std::is_same<typename std::allocator_traits<Allocator>::value_type,
                   posix_stacktrace_entry>::value);

  std::vector<posix_stacktrace_entry, Allocator> data_;

public:
  using value_type = posix_stacktrace_entry;
  using const_reference = const value_type &;
  using reference = value_type &;

  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using const_iterator =
      typename std::vector<posix_stacktrace_entry, Allocator>::const_iterator;
  using iterator = const_iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;

  basic_posix_stacktrace() noexcept(
      std::is_nothrow_default_constructible_v<Allocator>) = default;

  explicit basic_posix_stacktrace(const allocator_type &alloc) noexcept
      : data_(alloc)
  {
  }

  basic_posix_stacktrace(const basic_posix_stacktrace &other) = default;

  basic_posix_stacktrace(basic_posix_stacktrace &&other) noexcept = default;

  basic_posix_stacktrace(const basic_posix_stacktrace &other,
                         const allocator_type &alloc)
      : data_(other.data_, alloc)
  {
  }

  basic_posix_stacktrace(basic_posix_stacktrace &&other,
                         const allocator_type &alloc)
      : data_(std::move(other).data_, alloc)
  {
  }

  ~basic_posix_stacktrace() = default;

  basic_posix_stacktrace &operator=(const basic_posix_stacktrace &other);

  basic_posix_stacktrace &operator=(basic_posix_stacktrace &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value);

  static basic_posix_stacktrace
  current(const allocator_type &alloc = allocator_type()) noexcept;

  static basic_posix_stacktrace
  current(size_type skip,
          const allocator_type &alloc = allocator_type()) noexcept;

  static basic_posix_stacktrace
  current(size_type skip, size_type max_depth,
          const allocator_type &alloc = allocator_type()) noexcept;

  allocator_type get_allocator() const noexcept
  {
    return data_.get_allocator();
  }

  const_iterator begin() const noexcept { return data_.begin(); }
  const_iterator cbegin() const noexcept { return begin(); }

  const_iterator end() const noexcept { return data_.end(); }
  const_iterator cend() const noexcept { return end(); }

  reverse_const_iterator rbegin() const noexcept
  {
    return reverse_const_iterator(end());
  }
  reverse_const_iterator crbegin() const noexcept { return rbegin(); }

  reverse_const_iterator rend() const noexcept
  {
    return reverse_const_iterator(begin());
  }
  reverse_const_iterator crend() const noexcept { return rend(); }

  bool empty() const noexcept { return data_.empty(); }

  size_type size() const noexcept { return data_.size(); }

  size_type max_size() const noexcept { return data_.max_size(); }

  const_reference operator[](size_type pos) const { return data_[pos]; }

  const_reference at(size_type pos) const { return data_.at(pos); }

  void swap(basic_posix_stacktrace &other) noexcept(
      std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
      std::allocator_traits<Allocator>::is_always_equal::value)
  {
    data_.swap(other.data_);
  }

  bool operator==(const basic_posix_stacktrace &other)
  {
    return data_ == other.data_;
  }

  bool operator!=(const basic_posix_stacktrace &other)
  {
    return data_ != other.data_;
  }

  bool operator<(const basic_posix_stacktrace &other)
  {
    return data_ < other.data_;
  }
};

template <typename CharT, typename Traits, typename Allocator>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const basic_posix_stacktrace<Allocator> &st)
{
  typename std::basic_ostream<CharT, Traits>::sentry sentry(os);
  if (!sentry)
    return os;

  int width = 1;
  if (st.size() > 9)
    width = 2;
  if (st.size() > 99)
    width = 3;
  if (st.size() > 999)
    width = 4;

  int i(0);
  for (auto &e : st)
  {
    os  << std::setw(width) << ++i << "# " << e << '\n';
  }

  return os;
}

#ifdef GPCL_BFD

inline std::string posix_stacktrace_entry::source_file() const
{
  return bfd_stacktrace_entry_source_file(address);
}

inline std::uint_least32_t posix_stacktrace_entry::source_line() const
{
  return bfd_stacktrace_entry_source_line(address);
}

#endif

template <typename Allocator>
void swap(basic_posix_stacktrace<Allocator> &x,
          basic_posix_stacktrace<Allocator> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

constexpr std::size_t posix_stacktrace_impl_start_buffer_size = 100;

template <typename Allocator>
void posix_stacktrace_impl(
    size_t skip, size_t max_depth,
    std::vector<posix_stacktrace_entry, Allocator> &container)
{
  GPCL_TRY
  {
    std::vector<void *, typename std::allocator_traits<
                            Allocator>::template rebind_alloc<void *>>
        buffer(container.get_allocator());
    buffer.resize(posix_stacktrace_impl_start_buffer_size);

    std::size_t nframes;

    do
    {
      if (buffer.size() * 2 > max_depth)
        buffer.resize(max_depth);
      else
        buffer.resize(buffer.size() * 2);
      nframes = backtrace(buffer.data(), buffer.size());
    } while (nframes == buffer.size() && nframes < max_depth);

    for (std::size_t i = 0; i < nframes; ++i)
    {
      if (i >= skip)
      {
        container.emplace_back();
        container.back().address = buffer[i];
      }
    }
  }
  GPCL_CATCH(...) { container.clear(); }
  GPCL_CATCH_END
}

template <typename Allocator>
basic_posix_stacktrace<Allocator>
basic_posix_stacktrace<Allocator>::current(const Allocator &alloc) noexcept
{
  basic_posix_stacktrace st(alloc);
  posix_stacktrace_impl(0, size_t(-1), st.data_);
  return st;
}

template <typename Allocator>
basic_posix_stacktrace<Allocator>
basic_posix_stacktrace<Allocator>::current(size_type skip,
                                           const Allocator &alloc) noexcept
{
  basic_posix_stacktrace st(alloc);
  posix_stacktrace_impl(skip, size_t(-1), st.data_);
  return st;
}

template <typename Allocator>
basic_posix_stacktrace<Allocator>
basic_posix_stacktrace<Allocator>::current(size_type skip, size_type max_depth,
                                           const Allocator &alloc) noexcept
{
  basic_posix_stacktrace st(alloc);
  posix_stacktrace_impl(skip, max_depth, st.data_);
  return st;
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_POSIX_STACKTRACE_HPP
