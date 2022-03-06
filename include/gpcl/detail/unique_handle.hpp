//
// unique_handle.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_UNIQUE_HANDLE_HPP
#define GPCL_DETAIL_UNIQUE_HANDLE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/swap.hpp>

#ifdef GPCL_WINDOWS
#  include <Windows.h>
#else
#  include <unistd.h>
#endif

#include <cstdio>

namespace gpcl {

namespace detail {

template <typename HandleTraits>
class unique_handle : public HandleTraits
{
public:
  using traits_type = HandleTraits;
  using native_handle_type = typename HandleTraits::native_handle_type;

  explicit unique_handle(
      native_handle_type h = traits_type::invalid_value) noexcept
      : h_{h}
  {
  }

  unique_handle(const unique_handle &) = delete;
  unique_handle &operator=(const unique_handle &) = delete;

  unique_handle(unique_handle &&other) noexcept : unique_handle(other.release())
  {
  }
  unique_handle &operator=(unique_handle &&other) noexcept
  {
    reset(other.release());
    return *this;
  }

  ~unique_handle() { reset(); }

  void reset(native_handle_type h = traits_type::invalid_value) noexcept
  {
    if (traits_type::is_valid(h_))
      traits_type::close(h_);
    h_ = h;
  }

  native_handle_type release() noexcept
  {
    native_handle_type ret{h_};
    h_ = traits_type::invalid_value;
    return ret;
  }

  void swap(unique_handle &other) noexcept
  {
    using gpcl::swap;
    swap(h_, other.h_);
  }

  explicit operator bool() const noexcept { return traits_type::is_valid(h_); }

  native_handle_type get() const noexcept
  {
    GPCL_ASSERT(traits_type::is_valid(h_));
    return h_;
  }

  friend void swap(unique_handle &x, unique_handle &y) noexcept { x.swap(y); }

  friend bool operator<(const unique_handle &x, const unique_handle &y) noexcept
  {
    return x.get() < y.get();
  }

private:
  native_handle_type h_{traits_type::invalid_value};
};

#ifdef GPCL_WINDOWS
struct nullable_traits
{
  using native_handle_type = HANDLE;

  static constexpr native_handle_type invalid_value{nullptr};

  static constexpr bool is_valid(native_handle_type h) { return !!h; }

  static void close(native_handle_type h) noexcept
  {
    GPCL_VERIFY(::CloseHandle(h));
  }
};

using nullable_handle = unique_handle<nullable_traits>;

struct valid_traits
{
  using native_handle_type = HANDLE;

  static inline native_handle_type invalid_value{INVALID_HANDLE_VALUE};

  static bool is_valid(native_handle_type h) noexcept
  {
    return h != INVALID_HANDLE_VALUE;
  }

  static void close(native_handle_type h) noexcept
  {
    GPCL_VERIFY(::CloseHandle(h));
  }
};

using valid_handle = unique_handle<valid_traits>;

#endif

#ifdef GPCL_POSIX
struct fd_traits
{
  using native_handle_type = int;

  static constexpr native_handle_type invalid_value{-1};

  static constexpr bool is_valid(int fd) noexcept { return fd >= 0; }

  static void close(native_handle_type h) noexcept
  {
    GPCL_VERIFY(::close(h) == 0);
  }
};

using unique_fd = unique_handle<fd_traits>;

#endif

struct file_traits
{
  using native_handle_type = std::FILE *;

  static constexpr native_handle_type invalid_value{nullptr};

  static constexpr bool is_valid(native_handle_type h) noexcept { return !!h; }

  static void close(native_handle_type h) noexcept
  {
    GPCL_VERIFY(fclose(h) == 0);
  }
};

using unique_file = unique_handle<file_traits>;

} // namespace detail

} // namespace gpcl

#endif // GPCL_DETAIL_UNIQUE_HANDLE_HPP
