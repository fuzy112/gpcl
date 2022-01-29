//
// thread.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_THREAD_HPP
#define GPCL_THREAD_HPP

#include <gpcl/clock.hpp>
#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/thread_attributes.hpp>

#if defined GPCL_WINDOWS
#  include <gpcl/detail/win_thread.hpp>
#elif defined GPCL_POSIX
#  include <gpcl/detail/posix_thread.hpp>
#endif

#include <iostream>
#include <type_traits>

namespace gpcl {

#if defined(GPCL_POSIX)
using thread_id = detail::posix_thread_id;
#elif defined(GPCL_WINDOWS)
using thread_id = detail::win_thread_id;
#endif

/// Thread.
class thread
{
public:
#if defined(GPCL_POSIX)
  using impl_type = detail::posix_thread;
#elif defined(GPCL_WINDOWS)
  using impl_type = detail::win_thread;
#endif

  using native_handle_type = impl_type::native_handle_type;

  using attributes = thread_attributes;

  thread() noexcept = default;

  template <typename F, typename... Args>
  thread(const attributes &attr, F &&f, Args &&...args)
      : impl_(attr, detail::forward<F>(f), detail::forward<Args>(args)...)
  {
  }

  template <
      typename F, typename... Args,
      typename ::std::enable_if<
          !::std::is_convertible<F, const attributes &>::value, int>::type = 0>
  explicit thread(F &&f, Args &&...args)
      : thread(attributes{}, detail::forward<F>(f),
               detail::forward<Args>(args)...)
  {
  }

  thread(const thread &) = delete;
  auto operator=(thread const &) -> thread & = delete;

  thread(thread &&) noexcept = default;
  auto operator=(thread &&) noexcept -> thread & = default;

  auto swap(thread &other) noexcept -> void { impl_.swap(other.impl_); }

  [[nodiscard]] bool joinable() const { return impl_.joinable(); }

  void join() { return impl_.join(); }

  void detach() { return impl_.detach(); }

  [[nodiscard]] native_handle_type native_handle() const
  {
    return impl_.native_handle();
  }

  [[nodiscard]] thread_id get_id() const { return impl_.get_id(); }

  // extensions
#ifdef GPCL_POSIX
  auto name(czstring<> n) -> void { impl_.name(n); }
#endif

  static unsigned int hardware_concurrency()
  {
    return impl_type::hardware_concurrency();
  }

private:
  impl_type impl_;
};

inline auto swap(thread &y, thread &x) noexcept -> void
{
  x.swap(y);
}

namespace this_thread {
inline auto yield() -> void
{
  thread::impl_type::yield();
}

inline thread_id get_id()
{
  return thread::impl_type::this_thread_id();
}
} // namespace this_thread

inline auto join_all() -> void {}

template <typename Thread, typename... Rest>
inline auto join_all(Thread &t1, Rest &...rest) -> void
{
  t1.join();
  join_all(rest...);
}

} // namespace gpcl

#endif // GPCL_THREAD_HPP
