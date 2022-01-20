//
// win_thread.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_THREAD_HPP
#define GPCL_DETAIL_WIN_THREAD_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/thread_attributes.hpp>
#include <gpcl/unique_ptr.hpp>

#include <memory>
#include <ostream>
#include <type_traits>

#include <process.h>

namespace gpcl {
namespace detail {

class win_thread;

class win_thread_id
{
  DWORD value_ = 0;

  explicit win_thread_id(DWORD v) : value_(v) {}

public:
  constexpr win_thread_id() = default;

  constexpr explicit operator bool() const { return value_ != 0; }

  friend win_thread;

  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os, win_thread_id const &id)
  {
    typename std::basic_ostream<CharT, Traits>::sentry sentry(os);
    if (!os)
      return os;

    return os << '{' << std::hex << id.value_ << '}';
  }

  friend inline bool operator<(const win_thread_id &x, const win_thread_id &y)
  {
    return x.value_ < y.value_;
  }

  friend inline bool operator==(const win_thread_id &x, const win_thread_id &y)
  {
    return x.value_ == y.value_;
  }

  friend inline bool operator!=(const win_thread_id &x, const win_thread_id &y)
  {
    return x.value_ != y.value_;
  }
};

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
    (void)attr;
    start_thread(std::bind(f, args...));
  }

  win_thread(win_thread &&other) noexcept = default;

  auto operator=(win_thread &&other) noexcept -> win_thread & = default;

  auto swap(win_thread &other) noexcept -> void
  {
    using gpcl::swap;
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

  GPCL_DECL win_thread_id id() const;

  static GPCL_DECL void yield();

  static GPCL_DECL win_thread_id this_thread_id();

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

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/win_thread.ipp>
#endif

#endif // GPCL_DETAIL_WIN_THREAD_HPP
