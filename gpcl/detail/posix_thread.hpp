//
// posix_thread.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_THREAD_HPP
#define GPCL_DETAIL_POSIX_THREAD_HPP

#include <functional>
#include <memory>
#include <type_traits>

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/thread_attributes.hpp>
#include <gpcl/unique_ptr.hpp>
#include <gpcl/zstring.hpp>

#ifdef GPCL_POSIX
#  include <pthread.h>

namespace gpcl {
namespace detail {
GPCL_DECL void *posix_thread_function(void *arg) noexcept;

GPCL_DECL bool posix_thread_interrupted();

class posix_thread
{
public:
  using native_handle_type = pthread_t;

  GPCL_DECL posix_thread() noexcept = default;

  template <typename F, typename... Args>
  posix_thread(thread_attributes const &attr, F &&f, Args... args)
  {
    start_thread(attr, std::bind(detail::forward<F>(f), args...));
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

private:
  class func_base
  {
  public:
    virtual ~func_base() = default;

    virtual void run() noexcept = 0;
  };

  template <typename F>
  class func : public func_base
  {
  public:
    explicit func(F f) : f_(detail::move(f)) {}

    void run() noexcept final { f_(); }

  private:
    F f_;
  };

  GPCL_DECL void start_thread(thread_attributes const &attr,
                              unique_ptr<func_base> fn);

  template <typename F, decltype(std::declval<F>()(), int{}) = 0>
  void start_thread(thread_attributes const &attr, F &&f)
  {
    auto fn = gpcl::make_unique<func<F>>(detail::forward<F>(f));
    start_thread(attr, std::move(fn));
  }

  pthread_t thread_{};
  friend GPCL_DECL void *posix_thread_function(void *arg) noexcept;
};

} // namespace detail
} // namespace gpcl

#endif

#if defined(GPCL_HEADER_ONLY)
#  include <gpcl/detail/impl/posix_thread.ipp>
#endif

#endif // GPCL_DETAIL_POSIX_THREAD_HPP
