//
// future.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_FUTURE_HPP
#define GPCL_FUTURE_HPP

#include "gpcl/assert.hpp"
#include "gpcl/detail/config.hpp"
#include "gpcl/error.hpp"
#include "gpcl/event.hpp"
#include "gpcl/expected.hpp"
#include "gpcl/mutex.hpp"
#include "gpcl/shared_ptr.hpp"
#include "gpcl/thread.hpp"
#include "gpcl/unique_lock.hpp"
#include "gpcl/variant.hpp"

namespace gpcl {

namespace detail {
template <typename T>
class shared_state
{
  mutex mtx_;
  event ev_;
  variant<monostate, T, error_code> result_;

public:
  unique_lock<mutex> lock() { return unique_lock<mutex>(mtx_); }

  void set_value(unique_lock<mutex> &lock, T &&value)
  {
    GPCL_ASSERT(lock.owns_lock());
    GPCL_ASSERT(&lock.mutex() == &mtx_);
    GPCL_ASSERT(result_.index() == 0);
    result_.template emplace<1>(move(value));
    ev_.unlock_and_signal_one(lock);
  }

  template <typename... Args>
  void emplace_value(unique_lock<mutex> &lock, Args &&... args)
  {
    GPCL_ASSERT(lock.owns_lock());
    GPCL_ASSERT(&lock.mutex() == &mtx_);
    GPCL_ASSERT(result_.index() == 0);
    result_.template emplace<1>(std::forward<Args>(args)...);
    ev_.unlock_and_signal_one(lock);
  }

  template <typename... Args>
  void set_error(unique_lock<mutex> &lock, Args &&... args)
  {
    GPCL_ASSERT(lock.owns_lock());
    GPCL_ASSERT(&lock.mutex() == &mtx_);
    GPCL_ASSERT(result_.index() == 0);
    result_.template emplace<2>(std::forward<Args>(args)...);
    ev_.unlock_and_signal_one(lock);
  }

  T *wait(unique_lock<mutex> &lock, error_code &error)
  {
    GPCL_ASSERT(lock.owns_lock());
    GPCL_ASSERT(&lock.mutex() == &mtx_);
    GPCL_ASSERT(result_.index() == 0);
    ev_.wait(lock);
    return get(lock, error);
  }

  bool is_pending(unique_lock<mutex> &lock)
  {
    GPCL_ASSERT(lock.owns_lock());
    GPCL_ASSERT(&lock.mutex() == &mtx_);
    return result_.index() == 0;
  }

  T *get(unique_lock<mutex> &lock, error_code &error)
  {
    GPCL_ASSERT(lock.owns_lock());
    GPCL_ASSERT(&lock.mutex() == &mtx_);
    GPCL_ASSERT(result_.index() != 0);
    if (result_.index() == 1)
    {
      error = {};
      return &gpcl::get<T>(result_);
    }
    else if (result_.index() == 2)
    {
      error = gpcl::get<error_code>(result_);
    }

    GPCL_UNREACHABLE("invalid use of unlock and signal one");
  }
};
} // namespace detail

template <typename T>
class future;

template <typename T>
class promise
{
public:
  using value_type = T;
  using future_type = future<T>;

  template <typename... Args>
  void set_value(Args &&... args)
  {
    auto lock = state_->lock();
    state_->emplace_value(lock, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void set_error(Args &&... args)
  {
    auto lock = state_->lock();
    state_->set_error(lock, std::forward<Args>(args)...);
  }

  future_type get_future();

private:
  shared_ptr<detail::shared_state<T>> state_ =
      make_shared<detail::shared_state<T>>();
};

template <typename T>
class future
{
  friend class promise<T>;

  explicit future(shared_ptr<detail::shared_state<T>> state)
      : state_(std::move(state))
  {
  }

public:
  using promise_type = promise<T>;
  using value_type = T;

  T get()
  {
    error_code ec;
    auto lock = state_->lock();
    T *res = nullptr;
    if (state_->is_pending(lock))
    {
      res = state_->wait(lock, ec);
    }
    else
    {
      res = state_->get(lock, ec);
    }
    if (!ec)
    {
      return std::move(*res);
    }
    GPCL_THROW(system_error(ec, "get"));
  }

  T get(error_code &ec)
  {
    auto lock = state_->lock();
    T *res = nullptr;
    if (state_->is_pending(lock))
    {
      res = state_->wait(lock, ec);
    }
    else
    {
      res = state_->get(lock, ec);
    }
    if (!ec)
    {
      return std::move(*res);
    }
    return T{};
  }

  template <typename F>
  auto then(F &&f);

  template <typename F>
  auto catch_(F &&f);

private:
  shared_ptr<detail::shared_state<T>> state_;
};

template <typename T>
future<T> promise<T>::get_future()
{
  return future<T>{state_};
}

template <typename F>
future<std::invoke_result_t<F>> async(F &&f)
{
  promise<std::invoke_result_t<F>> promise_;
  auto ret = promise_.get_future();
  thread([f = std::forward<F>(f), promise_ = std::move(promise_)]() mutable {
    GPCL_TRY { promise_.set_value(f()); }
    GPCL_CATCH(system_error const &exc)
    {
#if !defined(GPCL_NO_EXCEPTIONS)
      promise_.set_error(exc.code());
#endif
    }
    GPCL_CATCH_END
  }).detach();
  return ret;
}

template <typename T>
template <typename F>
auto future<T>::then(F &&f)
{
  return async([f = std::forward<F>(f), self = std::move(*this)]() mutable {
    return f(self.get());
  });
}

template <typename T>
template <typename F>
auto future<T>::catch_(F &&f)
{
  return async([f = std::forward<F>(f), self = std::move(*this)]() mutable {
    error_code ec;
    auto t = self.get(ec);
    if (ec)
    {
      return f(ec);
    }
    return t;
  });
}

} // namespace gpcl

#endif // GPCL_FUTURE_HPP
