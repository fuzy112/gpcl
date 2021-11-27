//
// condition_variable.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CONDITION_VARIABLE_HPP
#define GPCL_CONDITION_VARIABLE_HPP

#include <gpcl/assert.hpp>
#include <gpcl/clock.hpp>
#include <gpcl/detail/posix_condition_variable.hpp>
#include <gpcl/detail/win_condition_variable.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/unique_lock.hpp>
#include <type_traits>

namespace gpcl {

#if defined(GPCL_POSIX) || defined(GPCL_WINDOWS)

enum class cv_status : bool
{
  no_timeout,
  timeout
};

/// Condition variable.
class condition_variable
{
public:
  /// Implementation defined type.
  using impl_type =
#  if defined(GPCL_POSIX)
      detail::posix_condition_variable;
#  elif defined(GPCL_WINDOWS)
      detail::win_condition_variable;
#  endif

  /// Default constructor.
  condition_variable() = default;

  condition_variable(const condition_variable &) = delete;
  condition_variable(condition_variable &&) = delete;

  condition_variable &operator=(const condition_variable &) = delete;
  auto operator=(condition_variable &&) -> condition_variable & = delete;

  void wait(unique_lock<mutex> &lock)
  {
    unique_lock_adaptor lock_adaptor(lock);
    impl_.wait(lock_adaptor.get_impl_lock());
  }

  template <typename Predicate>
  void wait(unique_lock<mutex> &lock, Predicate pred)
  {
    while (!pred())
    {
      wait(lock);
    }
  }

  template <typename Clock, typename Duration>
  cv_status wait_until(unique_lock<mutex> &lock,
                       system_time const &timeout_time)
  {
    unique_lock_adaptor lock_adaptor(lock);
    return static_cast<cv_status>(
        impl_.wait_until(lock_adaptor.get_impl_lock(), timeout_time));
  }

  cv_status wait_until(unique_lock<mutex> &lock,
                       system_time const &timeout_time)
  {
    unique_lock_adaptor lock_adaptor(lock);
    return static_cast<cv_status>(
        impl_.wait_for(lock_adaptor.get_impl_lock(), timeout_time.elapsed()));
  }

  cv_status wait_for(unique_lock<mutex> &lock, const duration &rel_time)
  {
    unique_lock_adaptor lock_adaptor(lock);
    return static_cast<cv_status>(
        impl_.wait_for(lock_adaptor.get_impl_lock(), rel_time));
  }

  template <typename Predicate>
  bool wait_until(unique_lock<mutex> &lock, const system_time &timeout_time,
                  Predicate pred)
  {
    while (!pred())
    {
      if (wait_until(lock, timeout_time) == cv_status::timeout)
        return pred();
    }
    GPCL_VERIFY(lock.owns_lock());
    return true;
  }

  template <typename Predicate>
  bool wait_for(unique_lock<mutex> &lock, duration const &rel_time,
                Predicate pred)
  {
    while (!pred())
    {
      if (wait_for(lock, rel_time) == cv_status::timeout)
        return pred();
    }
    GPCL_VERIFY(lock.owns_lock());
    return true;
  }

  void notify_one() { impl_.notify_one(); }

  void notify_all() { impl_.notify_all(); }

private:
  class unique_lock_adaptor
  {
  public:
    explicit unique_lock_adaptor(unique_lock<mutex> &lock)
        : lock_(lock),
          lock_mutex_impl_(lock.mutex().impl_, adopt_lock)
    {
      GPCL_ASSERT(lock_.owns_lock());
    }

    ~unique_lock_adaptor()
    {
      GPCL_ASSERT(lock_mutex_impl_.owns_lock());
      lock_mutex_impl_.release();
    }

    unique_lock<mutex::impl_type> &get_impl_lock() { return lock_mutex_impl_; }

  private:
    unique_lock<mutex> &lock_;
    unique_lock<mutex::impl_type> lock_mutex_impl_;
  };

  impl_type impl_;
};

#endif

} // namespace gpcl

#if defined(GPCL_HEADER_ONLY)
#  include <gpcl/detail/posix_condition_variable.hpp>
#endif

#endif
