//
// time.hpp
// ~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_TIME_HPP
#define GPCL_TIME_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/inttypes.hpp>
#include <gpcl/narrow_cast.hpp>
#include <limits>
#include <stdexcept>
#include <time.h>

namespace gpcl {

inline namespace time {

class duration;
class instant;
class system_time;
class system_time_error;

class duration
{
  u64 secs_{};
  u32 nanos_{};

public:
  constexpr duration() = default;

  inline constexpr duration(u64 secs, u32 nanos) : secs_(secs), nanos_(nanos)
  {
    GPCL_ASSERT_CONST(nanos <= 999'999'999);
  }

  inline static constexpr duration from_timespec(const timespec *ts)
  {
    GPCL_ASSERT_CONST(ts);
    return duration(ts->tv_sec, ts->tv_nsec);
  }

  inline static constexpr duration from_secs(u64 secs)
  {
    return duration(secs, 0);
  }

  inline static constexpr duration from_millis(u64 millis)
  {
    return duration(millis / 1000, millis % 1000 * 1'000'000);
  }

  inline static constexpr duration from_micros(u64 micros)
  {
    return duration(micros / 1'000'000, micros % 1'000'000 * 100);
  }

  inline static constexpr duration from_nanos(u64 nanos)
  {
    return duration(nanos / 1'000'000'000, nanos % 1'000'000'000);
  }

  inline constexpr bool is_zero() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);
    return secs_ == 0 && nanos_ == 0;
  }

  inline constexpr u64 as_secs() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);

    return secs_;
  }

  inline constexpr u32 subsec_millis() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);

    return nanos_ / 1'000;
  }

  inline constexpr u32 subsec_micros() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);

    return nanos_ / 1'000'000;
  }

  inline constexpr u32 subsec_nanos() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);

    return nanos_;
  }

  inline constexpr u64 as_millis() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);
    return u64(secs_) * 1000 + nanos_ / 1'000'000;
  }

  inline constexpr u64 as_micros() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);
    return u64(secs_) * 1'000'000 + nanos_ / 1'000;
  }

  inline constexpr u64 as_nanos() const
  {
    GPCL_ASSERT_CONST(nanos_ < 1'000'000'000);
    return u64(secs_) * 1'000'000'000 + nanos_;
  }

  duration checked_add(const duration &rhs) const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);
    GPCL_ASSERT(rhs.nanos_ < 1'000'000'000);
    u64 secs = secs_ + rhs.secs_;
    u32 nanos = nanos_ + rhs.nanos_;
    secs += nanos / 1'000'000'000;
    nanos = nanos % 1'000'000'000;
    if (secs < secs_ || secs < rhs.secs_)
    {
      GPCL_THROW(std::overflow_error("checked_add"));
    }
    return duration{secs, nanos};
  }

  duration saturating_add(const duration &rhs) const noexcept
  {
    u32 nanos = nanos_ + rhs.nanos_;
    u64 carry = nanos / 1'000'000'000;
    nanos = nanos % 1'000'000'000;
    u64 secs = carry + secs_ + rhs.secs_;
    if (secs < secs_ || secs < rhs.secs_)
    {
      return max;
    }
    else
    {
      return {secs, nanos};
    }
  }

  duration checked_sub(const duration &rhs) const
  {
    if (*this < rhs)
    {
      GPCL_THROW(std::overflow_error("checked_sub"));
    }

    if (nanos_ < rhs.nanos_)
    {
      return {secs_ - rhs.secs_ - 1, 1'000'000'000 + nanos_ - rhs.nanos_};
    }
    else
    {
      return {secs_ - rhs.secs_, nanos_ - rhs.nanos_};
    }
  }

  constexpr duration saturating_sub(const duration &rhs) const noexcept
  {
    if (*this < rhs)
    {
      return zero;
    }

    if (nanos_ < rhs.nanos_)
    {
      return {secs_ - rhs.secs_ - 1, 1'000'000'000 + nanos_ - rhs.nanos_};
    }
    else
    {
      return {secs_ - rhs.secs_, nanos_ - rhs.nanos_};
    }
  }

  inline constexpr timespec to_timespec() const
  {
    timespec retv{};
    retv.tv_sec = narrow_cast<time_t>(secs_);
    retv.tv_nsec = narrow_cast<long>(nanos_);
    return retv;
  }

  static inline constexpr duration from_timespec(const struct timespec &ts)
  {
    return duration{narrow_cast<u64>(ts.tv_sec), narrow_cast<u32>(ts.tv_nsec)};
  }

  friend inline constexpr bool operator==(const duration &lhs,
                                          const duration &rhs)
  {
    return lhs.secs_ == rhs.secs_ && lhs.nanos_ == rhs.nanos_;
  }

  friend inline constexpr bool operator!=(const duration &lhs,
                                          const duration &rhs)
  {
    return lhs.secs_ != rhs.secs_ || lhs.nanos_ != rhs.nanos_;
  }

  friend inline constexpr bool operator<(const duration &lhs,
                                         const duration &rhs)
  {
    return lhs.secs_ < rhs.secs_ ||
           (lhs.secs_ == rhs.secs_ && lhs.nanos_ < rhs.nanos_);
  }

  friend inline constexpr bool operator<=(const duration &lhs,
                                          const duration &rhs)
  {
    return lhs < rhs || lhs == rhs;
  }

  friend inline constexpr bool operator>(const duration &lhs,
                                         const duration &rhs)
  {
    return rhs < lhs && lhs != rhs;
  }

  friend inline constexpr bool operator>=(const duration &lhs,
                                          const duration &rhs)
  {
    return rhs < lhs || lhs == rhs;
  }

  static const duration second;
  static const duration millisecond;
  static const duration microsecond;
  static const duration nanosecond;
  static const duration zero;
  static const duration max;
};

class instant
{
  // duration since startup of the system.
  duration since_startup_;

  explicit instant(const duration &d) : since_startup_(d) {}

public:
  /// Returns an instant corresponding to "now".
  static inline instant now()
  {
#if defined GPCL_POSIX
    timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts))
    {
      detail::throw_system_error("clock_gettime");
    }

    return instant{duration::from_timespec(ts)};

#elif defined GPCL_WINDOWS
    GPCL_UNIMPLEMENTED();
#endif
  }

  /// Returns the amount of time elapsed from another instant to this one.
  inline constexpr duration duration_since(const instant &earlier) const;

  /// Returns the amount of time elapsed from another instant to this one.
  inline duration checked_duration_since(const instant &earlier) const
  {
    return since_startup_.checked_sub(earlier.since_startup_);
  }

  /// Returns the amount of time elapsed from another instant to this one, or
  /// zero duration if that instant is later than this one.
  inline constexpr duration
  saturating_duration_since(const instant &earlier) const
  {
    return since_startup_.saturating_sub(earlier.since_startup_);
  }

  /// Returns the amount of time elapsed since this instant was created.
  inline duration elapsed() const
  {
    return now().saturating_duration_since(*this);
  }

  friend inline constexpr bool operator==(const instant &lhs,
                                          const instant &rhs)
  {
    return lhs.since_startup_ == rhs.since_startup_;
  }

  friend inline constexpr bool operator!=(const instant &lhs,
                                          const instant &rhs)
  {
    return lhs.since_startup_ != rhs.since_startup_;
  }

  friend inline constexpr bool operator<(const instant &lhs, const instant &rhs)
  {
    return lhs.since_startup_ < rhs.since_startup_;
  }

  friend inline constexpr bool operator>(const instant &lhs, const instant &rhs)
  {
    return lhs.since_startup_ > rhs.since_startup_;
  }

  friend inline constexpr bool operator<=(const instant &lhs,
                                          const instant &rhs)
  {
    return lhs.since_startup_ <= rhs.since_startup_;
  }

  friend inline constexpr bool operator>=(const instant &lhs,
                                          const instant &rhs)
  {
    return lhs.since_startup_ >= rhs.since_startup_;
  }
};

class system_time
{
  // duration since startup of the system.
  duration since_epoch_;

  explicit system_time(const duration &d) : since_epoch_(d) {}

public:
  system_time() = default;

  /// Returns an system_time corresponding to "now".
  static inline system_time now()
  {
#if defined GPCL_POSIX
    timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts))
    {
      detail::throw_system_error("clock_gettime");
    }

    return system_time{duration::from_timespec(ts)};

#elif defined GPCL_WINDOWS
    GPCL_UNIMPLEMENTED();
#endif
  }

  /// Returns the amount of time elapsed from another system_time to this one.
  inline constexpr duration duration_since(const system_time &earlier) const;

  /// Returns the amount of time elapsed from another system_time to this one.
  inline duration checked_sub(const system_time &earlier) const
  {
    return since_epoch_.checked_sub(earlier.since_epoch_);
  }

  inline constexpr duration
  saturating_duration_since(const system_time &earlier) const
  {
    return since_epoch_.saturating_sub(earlier.since_epoch_);
  }

  inline system_time checked_add(const duration &dur) const
  {
    return system_time{since_epoch_.checked_add(dur)};
  }

  inline system_time checked_sub(const duration &dur) const
  {
    return system_time{since_epoch_.checked_sub(dur)};
  }

  /// Returns the amount of time elapsed since this system_time was created.
  inline duration elapsed() const
  {
    return now().saturating_duration_since(*this);
  }

  inline constexpr duration duration_since_epoch() const
  {
    return since_epoch_;
  }

  friend inline constexpr bool operator==(const system_time &lhs,
                                          const system_time &rhs)
  {
    return lhs.since_epoch_ == rhs.since_epoch_;
  }

  friend inline constexpr bool operator!=(const system_time &lhs,
                                          const system_time &rhs)
  {
    return lhs.since_epoch_ != rhs.since_epoch_;
  }

  friend inline constexpr bool operator<(const system_time &lhs,
                                         const system_time &rhs)
  {
    return lhs.since_epoch_ < rhs.since_epoch_;
  }

  friend inline constexpr bool operator>(const system_time &lhs,
                                         const system_time &rhs)
  {
    return lhs.since_epoch_ > rhs.since_epoch_;
  }

  friend inline constexpr bool operator<=(const system_time &lhs,
                                          const system_time &rhs)
  {
    return lhs.since_epoch_ <= rhs.since_epoch_;
  }

  friend inline constexpr bool operator>=(const system_time &lhs,
                                          const system_time &rhs)
  {
    return lhs.since_epoch_ >= rhs.since_epoch_;
  }

  static const system_time unix_epoch;
};

inline const duration duration::second = duration(1, 0);
inline const duration duration::millisecond = duration(0, 1000000);
inline const duration duration::microsecond = duration(0, 1000);
inline const duration duration::nanosecond = duration(0, 1);
inline const duration duration::zero = duration(0, 0);

inline const duration duration::max =
    duration((std::numeric_limits<u64>::max)(), 999'999'999);

inline const system_time system_time::unix_epoch;

} // namespace time

} // namespace gpcl

#endif // !GPCL_TIME_HPP
