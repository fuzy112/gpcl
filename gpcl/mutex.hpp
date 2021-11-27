//
// mutex.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MUTEX_HPP
#define GPCL_MUTEX_HPP

#include <gpcl/clock.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_mutex.hpp>
#include <gpcl/detail/win_mutex.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/thread_annotations.hpp>
#include <type_traits>

namespace gpcl {

class condition_variable;

/// Dummy mutex that does not synchronise any threads.
class GPCL_CAPABILITY("mutex") null_mutex : noncopyable
{
public:
  constexpr null_mutex() = default;
  ~null_mutex() = default;

  GPCL_ACQUIRE() auto lock() -> void
  {
    GPCL_ASSERT(!std::exchange(locked_, true));
  }

  GPCL_RELEASE() auto unlock() -> void
  {
    GPCL_ASSERT(std::exchange(locked_, false));
  }

  GPCL_TRY_ACQUIRE(true) auto try_lock() -> bool
  {
#ifdef GPCL_DEBUG
    return !std::exchange(locked_, true);
#else
    return true;
#endif
  }
  auto native_handle() noexcept -> std::nullptr_t { return nullptr; }

private:
  bool locked_{};
};

/// Normal mutex.
class GPCL_CAPABILITY("mutex") mutex : noncopyable
{
public:
#if defined(GPCL_DOXYGEN)
  using impl_type = /*unspecified*/;
#elif defined(GPCL_POSIX)
  using impl_type = detail::posix_normal_mutex;
#elif defined(GPCL_WINDOWS)
  using impl_type = detail::win_mutex;
#endif

  using native_handle_type = impl_type::native_handle_type;

  constexpr mutex() = default;

  mutex(const mutex &) = delete;
  mutex(mutex &&) = delete;

  auto operator=(const mutex &) -> mutex & = delete;
  auto operator=(mutex &&) -> mutex & = delete;

  GPCL_ACQUIRE() auto lock() -> void { return impl_.lock(); }
  GPCL_RELEASE() auto unlock() -> void { return impl_.unlock(); }
  GPCL_TRY_ACQUIRE(true) auto try_lock() -> bool { return impl_.try_lock(); }

  auto native_handle() noexcept -> native_handle_type
  {
    return impl_.native_handle();
  }

private:
  friend class condition_variable;

  impl_type impl_;
};

/// Timed mutex.
class GPCL_CAPABILITY("mutex") timed_mutex : noncopyable
{
public:
#if defined(GPCL_DOXYGEN)
  using impl_type = /*unspecified*/;
#elif defined(GPCL_POSIX)
  using impl_type = detail::posix_normal_mutex;
#elif defined(GPCL_WINDOWS)
  using impl_type = detail::win_timed_mutex;
#endif

  using native_handle_type = impl_type::native_handle_type;

  timed_mutex() = default;

  timed_mutex(const timed_mutex &) = delete;
  timed_mutex(timed_mutex &&) = delete;

  auto operator=(const timed_mutex &) -> timed_mutex & = delete;
  auto operator=(timed_mutex &&) -> timed_mutex & = delete;

  GPCL_ACQUIRE() auto lock() -> void { return impl_.lock(); }
  GPCL_RELEASE() auto unlock() -> void { return impl_.unlock(); }
  GPCL_TRY_ACQUIRE(true) auto try_lock() -> bool { return impl_.try_lock(); }

  template <typename Clock, typename Duration,
            typename std::enable_if<std::is_same<Clock, system_clock>::value,
                                    int>::type = 0>
  GPCL_TRY_ACQUIRE(true)
  auto try_lock_until(chrono::time_point<Clock, Duration> const &timeout_time)
      -> bool
  {
    return impl_.try_lock_until(
        chrono::time_point_cast<system_clock::duration>(timeout_time));
  }

  template <typename Clock, typename Duration,
            typename std::enable_if<!std::is_same<Clock, system_clock>::value,
                                    int>::type = 0>
  GPCL_TRY_ACQUIRE(true)
  auto try_lock_until(chrono::time_point<Clock, Duration> const &timeout_time)
      -> bool
  {
    return try_lock_for(timeout_time - Clock::now());
  }

  template <typename Rep, typename Period>
  GPCL_TRY_ACQUIRE(true)
  auto try_lock_for(chrono::duration<Rep, Period> const &rel_time) -> bool
  {
    return impl_.try_lock_for(
        chrono::duration_cast<system_clock::duration>(rel_time));
  }

  auto native_handle() noexcept -> native_handle_type
  {
    return impl_.native_handle();
  }

private:
  impl_type impl_;
};

/// Reentrant mutex.
class recursive_mutex : noncopyable
{
public:
#if defined(GPCL_DOXYGEN)
  using impl_type = /*unspecified*/;
#elif defined(GPCL_POSIX)
  using impl_type = detail::posix_recursive_mutex;
#elif defined(GPCL_WINDOWS)
  using impl_type = detail::win_recursive_mutex;
#endif

  using native_handle_type = impl_type::native_handle_type;

  recursive_mutex() = default;

  recursive_mutex(const recursive_mutex &) = delete;
  recursive_mutex(recursive_mutex &&) = delete;

  auto operator=(const recursive_mutex &) -> recursive_mutex & = delete;
  auto operator=(recursive_mutex &&) -> recursive_mutex & = delete;

  GPCL_ACQUIRE() auto lock() -> void { return impl_.lock(); }
  GPCL_RELEASE() auto unlock() -> void { return impl_.unlock(); }
  GPCL_TRY_ACQUIRE(true) auto try_lock() -> bool { return impl_.try_lock(); }

  auto native_handle() noexcept -> native_handle_type
  {
    return impl_.native_handle();
  }

private:
  impl_type impl_;
};

} // namespace gpcl

#endif
