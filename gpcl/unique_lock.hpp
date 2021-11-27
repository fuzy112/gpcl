//
// unique_lock.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_UNIQUE_LOCK_HPP
#define GPCL_UNIQUE_LOCK_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/is_basic_lockable.hpp>
#include <gpcl/is_lockable.hpp>
#include <gpcl/thread_annotations.hpp>
#include <utility>

namespace gpcl {

struct adopt_lock_t
{
};

struct try_to_lock_t
{
};

struct defer_lock_t
{
};

GPCL_CXX17_INLINE_CONSTEXPR adopt_lock_t adopt_lock{};
GPCL_CXX17_INLINE_CONSTEXPR try_to_lock_t try_to_lock{};
GPCL_CXX17_INLINE_CONSTEXPR defer_lock_t defer_lock{};

/// RAII type that automatically unlocks the mutex.
template <typename MutexType>
class GPCL_SCOPED_CAPABILITY unique_lock
{
  static_assert(is_basic_lockable<MutexType>::value,
                "MutexType should meet BasicLockable");

public:
  using mutex_type = MutexType;

  explicit unique_lock(mutex_type &mtx) GPCL_ACQUIRE(mtx)
      : mtx_(&mtx),
        owns_lock_(false)
  {
    lock();
  }

  unique_lock(mutex_type &mtx, defer_lock_t) noexcept GPCL_EXCLUDES(mtx)
      : mtx_(&mtx),
        owns_lock_(false)
  {
  }

  unique_lock(mutex_type &mtx, adopt_lock_t) noexcept GPCL_REQUIRES(mtx)
      : mtx_(&mtx),
        owns_lock_(true)
  {
  }

  template <typename T = mutex_type,
            typename std::enable_if<is_lockable<T>::value, int>::type = 0>
  unique_lock(mutex_type &mtx, try_to_lock_t) : mtx_(&mtx),
                                                owns_lock_(false)
  {
    try_lock();
  }

  unique_lock(const unique_lock &) = delete;
  auto operator=(const unique_lock &) -> unique_lock & = delete;

  unique_lock(unique_lock &&other) noexcept
      : mtx_(other.release()),
        owns_lock_(true)
  {
  }

  ~unique_lock() GPCL_RELEASE()
  {
    if (owns_lock_)
      mtx_->unlock();
  }

  auto operator=(unique_lock &&other) noexcept -> unique_lock &
  {
    using std::swap;
    swap(other);
  }

  auto swap(unique_lock &other) noexcept -> void
  {
    using std::swap;
    swap(mtx_, other.mtx_);
    swap(owns_lock_, other.owns_lock_);
  }

  friend inline auto swap(unique_lock &x, unique_lock &y) noexcept -> void
  {
    x.swap(y);
  }

  [[nodiscard]] auto owns_lock() const noexcept -> bool { return owns_lock_; }

  [[nodiscard]] mutex_type &mutex() const noexcept { return *mtx_; }

  auto release() noexcept -> void
  {
    GPCL_ASSERT(owns_lock_);
    GPCL_ASSERT(mtx_);
    owns_lock_ = false;
    mtx_ = nullptr;
  }

  GPCL_ACQUIRE() void lock()
  {
    GPCL_ASSERT(owns_lock_ == false);
    mutex().lock();
    owns_lock_ = true;
  }

  template <typename T = mutex_type,
            typename std::enable_if<is_lockable<T>::value, int>::type = 0>
  GPCL_TRY_ACQUIRE(true)
  bool try_lock()
  {
    GPCL_ASSERT(owns_lock_ == false);
    owns_lock_ = mutex().try_lock();
    return owns_lock_;
  }

  GPCL_RELEASE() void unlock()
  {
    GPCL_ASSERT(owns_lock_);
    mutex().unlock();
    owns_lock_ = false;
  }

private:
  mutex_type *mtx_{};
  bool owns_lock_{};
};

} // namespace gpcl

#endif // GPCL_UNIQUE_LOCK_HPP
