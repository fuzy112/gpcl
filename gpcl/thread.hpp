//
// thread.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_THREAD_HPP
#define GPCL_THREAD_HPP

#include <gpcl/clock.hpp>
#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/posix_thread.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/detail/win_thread.hpp>
#include <gpcl/thread_attributes.hpp>
#include <type_traits>

namespace gpcl {

#if defined(GPCL_POSIX) || defined(GPCL_WINDOWS)

/// Thread.
class thread
{
public:
#  if defined(GPCL_POSIX)
  using impl_type = detail::posix_thread;
#  elif defined(GPCL_WINDOWS)
  using impl_type = detail::win_thread;
#  endif

  using native_handle_type = impl_type::native_handle_type;

  using attributes = thread_attributes;

  thread() noexcept = default;

  template <typename F, typename... Args>
  thread(const attributes &attr, F &&f, Args &&... args)
      : impl_(attr, detail::forward<F>(f), detail::forward<Args>(args)...)
  {
  }

  template <
      typename F, typename... Args,
      typename ::std::enable_if<
          !::std::is_convertible<F, const attributes &>::value, int>::type = 0>
  explicit thread(F &&f, Args &&... args)
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

  // extensions
#  ifdef GPCL_POSIX
  auto name(czstring<> n) -> void { impl_.name(n); }
#  endif

private:
  impl_type impl_;
};

#endif

namespace detail {
inline auto swap(thread &y, thread &x) noexcept -> void
{
  x.swap(y);
}
} // namespace detail

namespace this_thread {

#ifdef GPCL_POSIX
inline auto yield() -> void
{
  thread::impl_type::yield();
}
#endif

} // namespace this_thread

inline auto join_all() -> void {}

template <typename Thread, typename... Rest>
inline auto join_all(Thread &t1, Rest &... rest) -> void
{
  t1.join();
  join_all(rest...);
}

} // namespace gpcl

#endif
