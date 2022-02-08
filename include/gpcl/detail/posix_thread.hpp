//
// posix_thread.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_THREAD_HPP
#define GPCL_DETAIL_POSIX_THREAD_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/thread_attributes.hpp>
#include <gpcl/unique_ptr.hpp>
#include <gpcl/zstring.hpp>
#include <gpcl/bind_front.hpp>
#include <gpcl/function.hpp>

#include <iostream>
#include <type_traits>

#include <pthread.h>
#include <sys/types.h>

namespace gpcl {
namespace detail {

class posix_thread;

GPCL_DECL void *posix_thread_function(void *arg) noexcept;

GPCL_DECL bool posix_thread_interrupted();

class posix_thread_id
{
#if defined GPCL_CONFIG_POSIX_THREAD_ID_IS_TID
  pid_t value_ = 0;

  explicit posix_thread_id(pid_t id) : value_(id) {}

#else
  pthread_t value_ = 0;

  explicit posix_thread_id(pthread_t id) : value_(id) {}

#endif

  friend posix_thread;

public:
  constexpr posix_thread_id() = default;

  constexpr explicit operator bool() const { return value_ != 0; }

  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os, posix_thread_id const &id)
  {
    typename std::basic_ostream<CharT, Traits>::sentry sentry(os);
    if (!os)
      return os;

    return os << '{' << std::hex << id.value_ << '}';
  }

  friend bool operator<(const posix_thread_id &x, const posix_thread_id &y)
  {
    return x.value_ < y.value_;
  }

  friend bool operator==(const posix_thread_id &x, const posix_thread_id &y)
  {
    return x.value_ == y.value_;
  }

  friend bool operator!=(const posix_thread_id &x, const posix_thread_id &y)
  {
    return x.value_ != y.value_;
  }
};

class posix_thread
{
public:
  using native_handle_type = pthread_t;

  GPCL_DECL posix_thread() noexcept = default;

  template <typename F, typename... Args>
  posix_thread(thread_attributes const &attr, F &&f, Args... args)
  {
    start_thread(attr, gpcl::bind_front(detail::forward<F>(f), args...));
  }

  GPCL_DECL ~posix_thread();

  posix_thread(const posix_thread &) = delete;
  auto operator=(posix_thread const &) -> posix_thread & = delete;

  posix_thread(posix_thread &&other) noexcept
  {
    thread_ = other.thread_;
    other.thread_ = 0;
  }

  auto operator=(posix_thread &&other) noexcept -> posix_thread &
  {
    this->swap(other);
    return *this;
  }

  auto swap(posix_thread &other) noexcept -> void
  {
    using gpcl::swap;
    swap(thread_, other.thread_);
  }

  friend inline auto swap(posix_thread &x, posix_thread &y) noexcept -> void
  {
    x.swap(y);
  }

  static GPCL_DECL auto yield() -> void;

  GPCL_DECL void name(czstring<> s);

  [[nodiscard]] bool joinable() const { return thread_ != 0; }

  GPCL_DECL void join();

  GPCL_DECL void detach();

  [[nodiscard]] native_handle_type native_handle() const { return thread_; }

  GPCL_DECL posix_thread_id get_id() const;

  static GPCL_DECL posix_thread_id this_thread_id();

  static GPCL_DECL unsigned int hardware_concurrency();

private:
  GPCL_DECL void start_thread(thread_attributes const &attr,
                              unique_ptr<function<void()>> fn);

  template <typename F, decltype(std::declval<F>()(), int{}) = 0>
  void start_thread(thread_attributes const &attr, F &&f)
  {
    auto fn = gpcl::make_unique<function<void()>>(detail::forward<F>(f));
    start_thread(attr, std::move(fn));
  }

  pthread_t thread_{};
  friend GPCL_DECL void *posix_thread_function(void *arg) noexcept;
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_THREAD_HPP
