//
// event.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EVENT_HPP
#define GPCL_EVENT_HPP

#include <gpcl/assert.hpp>
#include <gpcl/condition_variable.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/unique_lock.hpp>

namespace gpcl {

/// A never-spurious-wakeup condition variable.
class event : noncopyable
{
public:
  /// Constructor.
  event() : signaled_(0), waiters_(0) {}

  /// Destructor.
  ~event() noexcept { GPCL_ASSERT(std::size_t(waiters_) == 0); }

  /// Clear the signaled state.
  template <typename Lock>
  void reset(Lock &lock) noexcept
  {
    (void)lock;
    GPCL_ASSERT(lock.owns_lock());
    signaled_ = 0;
  }

  /// Wakeup all waiters.
  template <typename Lock>
  void signal_all(Lock &lock)
  {
    GPCL_ASSERT(lock.owns_lock());
    (void)lock;
    signaled_ = 1;
    cond_.notify_all();
  }

  /// Unlock the lock and wakeup one waiter (if any).
  template <typename Lock>
  void unlock_and_signal_one(Lock &lock)
  {
    GPCL_ASSERT(lock.owns_lock());
    signaled_ = 1;
    bool has_waiters = waiters_;
#ifndef GPCL_XENOMAI
    lock.unlock();
    if (has_waiters)
      cond_.notify_one();
#else
    if (has_waiters)
      cond_.notify_one();
    lock.unlock();
#endif
  }

  /// Wakeup one waiter.
  ///
  /// This is used when the waiter wants to destroy the event.
  template <typename Lock>
  void signal_one(Lock &lock)
  {
    GPCL_ASSERT(lock.owns_lock());
    (void)lock;
    if (waiters_ != 0)
    {
      signaled_ = true;
      cond_.notify_one();
    }
  }

  /// If there is a waiter, then unlock and wakeup it.
  /// Otherwise do nothing.
  ///
  /// \returns Whether a waiter has been waken up.
  template <typename Lock>
  bool try_unlock_and_signal_one(Lock &lock)
  {
    GPCL_ASSERT(lock.owns_lock());
    signaled_ = true;
    if (waiters_ != 0)
    {
      lock.unlock();
      cond_.notify_one();
      return true;
    }
    return false;
  }

  /// Wait for signaling.
  void wait(unique_lock<mutex> &lock)
  {
    GPCL_ASSERT(lock.owns_lock());
    while (!signaled_)
    {
      waiter w(this);
      cond_.wait(lock);
    }
  }

  /// Wait for signaling.
  template <typename Rep, typename Clock>
  bool wait_for(unique_lock<mutex> &lock,
                chrono::duration<Rep, Clock> const &timeout)
  {
    GPCL_ASSERT(lock.owns_lock());
    if (!signaled_)
    {
      waiter w(this);
      cond_.wait_for(lock, timeout);
    }
    return signaled_;
  }

  /// Wait for signaling.
  template <typename Duration, typename Clock>
  bool wait_until(unique_lock<mutex> &lock,
                  chrono::time_point<Clock, Duration> const &timeout)
  {
    GPCL_ASSERT(lock.owns_lock());
    if (!signaled_)
    {
      waiter w(this);
      cond_.wait_until(lock, timeout);
    }
    return signaled_;
  }

private:
  // RAII helper that automatically modifies the waiter count.
  class waiter : noncopyable
  {
  public:
    explicit waiter(event *e) noexcept : e_(e) { ++e_->waiters_; }

    ~waiter() noexcept { --e_->waiters_; }

  private:
    event *e_;
  };

  condition_variable cond_;

  // Signaled flag.
  std::size_t signaled_ : 1;

  // Number of waiters.
  std::size_t waiters_ : sizeof(std::size_t) * 8 - 1;
};

} // namespace gpcl

#endif // GPCL_EVENT_HPP
