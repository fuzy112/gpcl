//
// win_stacktrace.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_STACKTRACE_HPP
#define GPCL_DETAIL_WIN_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/dynarray.hpp>
#include <gpcl/error.hpp>

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <type_traits>

#if defined(GPCL_NO_STACKTRACE)
#  error "This header should not be included."
#endif

#include <windows.h>

#if defined _MSC_VER && !defined GPCL_SOURCE &&                                \
    !defined GPCL_DISABLE_AUTO_LINKING && !defined(GPCL_NO_STACKTRACE)
#  pragma comment(lib, "dbghelp")
#endif

namespace gpcl {
template <typename MutexType>
class unique_lock;
}

namespace gpcl::detail {

class win_recursive_mutex;

template <typename Allocator>
class basic_win_stacktrace;

class win_stacktrace_entry;

class win_stacktrace_entry
{
  PVOID addr_ = 0;

public:
  using native_handle_type = PVOID;

  constexpr win_stacktrace_entry() noexcept = default;

  constexpr win_stacktrace_entry(native_handle_type address) : addr_(address) {}

  constexpr win_stacktrace_entry(const win_stacktrace_entry &) = default;

  constexpr win_stacktrace_entry &
  operator=(const win_stacktrace_entry &) = default;

  constexpr native_handle_type native_handle() const noexcept { return addr_; }

  constexpr explicit operator bool() const noexcept { return !!addr_; }

  GPCL_DECL std::string description() const;

  GPCL_DECL std::string source_file() const;

  GPCL_DECL std::string binary_file() const;

  GPCL_DECL std::uint_least32_t source_line() const;

  friend inline bool operator<(const win_stacktrace_entry &x,
                               const win_stacktrace_entry &y)
  {
    return x.addr_ < y.addr_;
  }

  friend inline bool operator!=(const win_stacktrace_entry &x,
                                const win_stacktrace_entry &y)
  {
    return x.addr_ != y.addr_;
  }

  friend inline bool operator==(const win_stacktrace_entry &x,
                                const win_stacktrace_entry &y)
  {
    return !(x != y);
  }

  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os,
             const win_stacktrace_entry &f)
  {
#if defined(GPCL_BFD) || defined(__CYGWIN__)
    return os << f.description();
#else
    typename std::basic_ostream<CharT, Traits>::sentry sentry(os);
    if (!sentry)
      return os;

    if (!f)
      return os << "[empty]";

    auto desc = f.description();
    auto file = f.source_file();
    auto line = f.source_line();

    if (desc.empty())
      os << f.native_handle();
    else
      os << desc;

    if (line)
      return os << " at " << file << '(' << std::dec << line << ')';

    auto binary = f.binary_file();
    if (!binary.empty())
      os << " in " << binary;

    return os;
#endif
  }
};

template <typename Allocator>
class basic_win_stacktrace
{
  static_assert(
      std::is_same<typename std::allocator_traits<Allocator>::value_type,
                   win_stacktrace_entry>::value);

  gpcl::dynarray<win_stacktrace_entry, Allocator> data_;

public:
  using value_type = win_stacktrace_entry;
  using const_reference = const value_type &;
  using reference = value_type &;

  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using const_iterator =
      typename gpcl::dynarray<win_stacktrace_entry, Allocator>::const_iterator;
  using iterator = const_iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;

  basic_win_stacktrace() noexcept(
      std::is_nothrow_default_constructible_v<Allocator>) = default;

  explicit basic_win_stacktrace(const allocator_type &alloc) noexcept
      : data_(alloc)
  {
  }

  basic_win_stacktrace(const basic_win_stacktrace &other) = default;

  basic_win_stacktrace(basic_win_stacktrace &&other) noexcept = default;

  basic_win_stacktrace(const basic_win_stacktrace &other,
                       const allocator_type &alloc)
      : data_(other.data_, alloc)
  {
  }

  basic_win_stacktrace(basic_win_stacktrace &&other,
                       const allocator_type &alloc)
      : data_(std::move(other).data_, alloc)
  {
  }

  ~basic_win_stacktrace() = default;

  basic_win_stacktrace &operator=(const basic_win_stacktrace &other);

  basic_win_stacktrace &operator=(basic_win_stacktrace &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value);

  static __forceinline basic_win_stacktrace
  current(const allocator_type &alloc = allocator_type()) noexcept;

  static __forceinline basic_win_stacktrace
  current(size_type skip,
          const allocator_type &alloc = allocator_type()) noexcept;

  static __forceinline basic_win_stacktrace
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

  void swap(basic_win_stacktrace &other) noexcept(
      std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
      std::allocator_traits<Allocator>::is_always_equal::value)
  {
    data_.swap(other.data_);
  }

  bool operator==(const basic_win_stacktrace &other)
  {
    return data_ == other.data_;
  }

  bool operator!=(const basic_win_stacktrace &other)
  {
    return data_ != other.data_;
  }

  bool operator<(const basic_win_stacktrace &other)
  {
    return data_ < other.data_;
  }
};

template <typename CharT, typename Traits, typename Allocator>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const basic_win_stacktrace<Allocator> &st)
{
  typename std::basic_ostream<CharT, Traits>::sentry sentry(os);
  if (!sentry)
    return os;

  os << '\n';

  int i(0);
  for (auto &e : st)
  {
    os << std::setw(4) << std::dec << ++i << "# " << e << '\n';
  }

  return os;
}

template <typename Allocator>
void swap(basic_win_stacktrace<Allocator> &x,
          basic_win_stacktrace<Allocator> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

#if !defined(__CYGWIN__) && !defined(GPCL_BFD)
struct win_dbg_helper
{
private:
  GPCL_DECL win_dbg_helper();

  GPCL_DECL ~win_dbg_helper();

public:
  GPCL_DECL unique_lock<win_recursive_mutex> lock();

  HANDLE process() const { return GetCurrentProcess(); }

  static GPCL_DECL win_dbg_helper &instance();
};

static auto &g_win_dbg_helper = win_dbg_helper::instance();
#endif

template <typename Allocator>
basic_win_stacktrace<Allocator>
basic_win_stacktrace<Allocator>::current(size_type skip, size_type max_depth,
                                         const Allocator &alloc) noexcept
{
  basic_win_stacktrace result(alloc);
  win_stacktrace_impl(skip, max_depth, result.data_);
  return result;
}

template <typename Allocator>
basic_win_stacktrace<Allocator>
basic_win_stacktrace<Allocator>::current(size_type skip,
                                         const Allocator &alloc) noexcept
{
  basic_win_stacktrace result(alloc);
  win_stacktrace_impl(skip, size_type(-1), result.data_);
  return result;
}

template <typename Allocator>
basic_win_stacktrace<Allocator>
basic_win_stacktrace<Allocator>::current(const Allocator &alloc) noexcept
{
  basic_win_stacktrace result(alloc);
  win_stacktrace_impl(0, size_type(-1), result.data_);
  return result;
}

} // namespace gpcl::detail

#include <gpcl/detail/impl/win_stacktrace.hpp>

#endif // GPCL_DETAIL_WIN_STACKTRACE_HPP
