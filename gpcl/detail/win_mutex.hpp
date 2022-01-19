//
// win_mutex.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/detail/win_clock.hpp>
#include <gpcl/detail/win_once_flag.hpp>
#include <gpcl/noncopyable.hpp>

#include <windows.h>

namespace gpcl {
namespace detail {

class win_recursive_mutex : noncopyable
{
  win_once_flag once_;
  CRITICAL_SECTION cs_{};

public:
  using native_handle_type = LPCRITICAL_SECTION;

private:
  GPCL_DECL void init() noexcept;

public:
  constexpr win_recursive_mutex() = default;

  GPCL_DECL ~win_recursive_mutex() noexcept;

  GPCL_DECL void lock() noexcept;

  GPCL_DECL void unlock() noexcept;

  GPCL_DECL auto try_lock() noexcept -> bool;

  auto native_handle() noexcept -> native_handle_type { return &cs_; }
};

class win_condition_variable;

class win_mutex : win_recursive_mutex
{
public:
  using win_recursive_mutex::native_handle_type;

  using win_recursive_mutex::win_recursive_mutex;

  GPCL_DECL void lock();

  GPCL_DECL void unlock();

  GPCL_DECL auto try_lock() -> bool;

  using win_recursive_mutex::native_handle;
};

class win_timed_mutex : noncopyable
{
  null_handle mtx_;

public:
  using native_handle_type = ::HANDLE;

  GPCL_DECL win_timed_mutex();
  ~win_timed_mutex() noexcept = default;

  GPCL_DECL auto lock() -> void;

  GPCL_DECL auto unlock() -> void;

  GPCL_DECL auto try_lock() -> bool;

  GPCL_DECL auto try_lock_for(system_clock::duration dur) -> bool;

  GPCL_DECL auto try_lock_until(system_clock::time_point tp) -> bool;

  auto native_handle() noexcept -> native_handle_type { return mtx_.get(); }
};
} // namespace detail
} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/win_mutex.ipp>
#endif
