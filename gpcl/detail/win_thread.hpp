//
// win_thread.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_THREAD_HPP
#define GPCL_DETAIL_WIN_THREAD_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/thread_attributes.hpp>
#include <memory>
#include <type_traits>

#ifdef GPCL_WINDOWS
#  include <process.h>
#  include <windows.h>

namespace gpcl {
namespace detail {

typedef std::remove_pointer<_beginthreadex_proc_type>::type
    win_thread_proc_type;

GPCL_DECL win_thread_proc_type win_thread_proc;

class win_thread
{
public:
  ::HANDLE typedef native_handle_type;

  win_thread() = default;

  template <typename F, typename... Args>
  explicit win_thread(thread_attributes const &attr, F &&f, Args... args)
  {
    start_thread(std::bind(f, args...));
  }

  win_thread(win_thread &&other) noexcept = default;

  auto operator=(win_thread &&other) noexcept -> win_thread & = default;

  auto swap(win_thread &other) noexcept -> void
  {
    using std::swap;
    swap(thread_, other.thread_);
  }

  friend inline auto swap(win_thread &x, win_thread &y) noexcept -> void
  {
    x.swap(y);
  }

  GPCL_DECL ~win_thread();

  GPCL_DECL auto detach() -> void;

  GPCL_DECL auto join() -> void;

  auto joinable() const -> bool { return !!thread_; }

  auto native_handle() const -> native_handle_type { return thread_.get(); }

private:
  class func_base
  {
  public:
    virtual ~func_base() = default;

    virtual void run() = 0;
  };

  template <typename F>
  class func : public func_base
  {
  public:
    explicit func(F f) : f_(detail::move(f)) {}

    void run() final { f_(); }

  private:
    F f_;
  };

  GPCL_DECL void start_thread_impl(func_base *fn);

  template <typename F, decltype(std::declval<F>()(), int{}) = 0>
  void start_thread(F &&f)
  {
    auto fn = unique_ptr<func_base>(new func<F>(detail::forward<F>(f)));
    this->start_thread_impl(fn.release());
  }

  null_handle thread_;

  friend GPCL_DECL win_thread_proc_type win_thread_proc;
};

} // namespace detail
} // namespace gpcl

#endif

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/win_thread.ipp>
#endif

#endif
