//
// posix_mutex.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_MUTEX_HPP
#define GPCL_DETAIL_POSIX_MUTEX_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/detail/posix_clock.hpp>

#include <pthread.h>

namespace gpcl {
namespace detail {


enum class posix_mutex_protocol
{
  prio_inherit = PTHREAD_PRIO_INHERIT,
  prio_none = PTHREAD_PRIO_NONE,
  prio_protect = PTHREAD_PRIO_PROTECT,
};

enum class posix_mutex_robust
{
  stalled = PTHREAD_MUTEX_STALLED, // no action if thread terminated while
                                   // holding the mutex

  robust = PTHREAD_MUTEX_ROBUST, // robust
};

enum class posix_mutex_type
{
  normal = PTHREAD_MUTEX_NORMAL,
  error_check = PTHREAD_MUTEX_ERRORCHECK,
  recursive = PTHREAD_MUTEX_RECURSIVE,
  default_ = PTHREAD_MUTEX_DEFAULT,
};

class posix_mutex_attr
{
public:
  GPCL_DECL posix_mutex_attr();
  GPCL_DECL ~posix_mutex_attr() noexcept;

  posix_mutex_attr(const posix_mutex_attr &) = delete;
  posix_mutex_attr &operator=(const posix_mutex_attr &) = delete;

  [[nodiscard]] GPCL_DECL posix_mutex_protocol protocol() const noexcept;
  GPCL_DECL void protocol(posix_mutex_protocol p) noexcept;

  [[nodiscard]] GPCL_DECL int priority_ceiling() const noexcept;
  GPCL_DECL void priority_ceiling(int prioceiling) noexcept;

  [[nodiscard]] GPCL_DECL posix_mutex_robust robust() const noexcept;
  GPCL_DECL void robust(posix_mutex_robust r) noexcept;

  [[nodiscard]] GPCL_DECL posix_mutex_type type() const noexcept;
  GPCL_DECL void type(posix_mutex_type t) noexcept;

  [[nodiscard]] const pthread_mutexattr_t *get() const { return &attr_; }

private:
  pthread_mutexattr_t attr_{};
};

class posix_mutex_base
{
protected:
  GPCL_DECL ~posix_mutex_base();

public:
  using native_handle_type = pthread_mutex_t *;

  constexpr posix_mutex_base() : mtx_(PTHREAD_MUTEX_INITIALIZER) {}

  GPCL_DECL explicit posix_mutex_base(const posix_mutex_attr &attr);

  GPCL_DECL void lock();

  GPCL_DECL void unlock();

  GPCL_DECL bool try_lock();

  GPCL_DECL bool try_lock_for(realtime_clock::duration dur);

  GPCL_DECL bool try_lock_until(realtime_clock::time_point tp);

  native_handle_type native_handle() noexcept { return &mtx_; }

private:
  pthread_mutex_t mtx_{};
};

class posix_normal_mutex : public posix_mutex_base
{
public:
  constexpr posix_normal_mutex() : posix_mutex_base() {}
};

class posix_recursive_mutex : public posix_mutex_base
{
  static auto init_attr(posix_mutex_attr &&attr = posix_mutex_attr())
      -> posix_mutex_attr &&
  {
    attr.type(posix_mutex_type::recursive);
#if defined(GPCL_DEBUG)
    attr.robust(posix_mutex_robust::robust);
#endif
    return detail::move(attr);
  }

public:
  posix_recursive_mutex() : posix_mutex_base(init_attr()) {}
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_MUTEX_HPP
