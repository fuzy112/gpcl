//
// posix_clock.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_CLOCK_HPP
#define GPCL_DETAIL_POSIX_CLOCK_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/config.hpp>

#include <ctime>

namespace gpcl {
namespace detail {

inline chrono::nanoseconds to_duration(const struct timespec *ts) noexcept
{
  return chrono::seconds(ts->tv_sec) + chrono::nanoseconds(ts->tv_nsec);
}

inline chrono::nanoseconds to_duration(const struct timespec &ts) noexcept
{
  return to_duration(&ts);
}

inline struct timespec to_timespec(const chrono::nanoseconds &duration) noexcept
{
  auto ts = timespec{};
  ts.tv_sec = duration.count() / 1000000000;
  ts.tv_nsec = duration.count() % 1000000000;
  return ts;
}

struct monotonic_clock
{
  monotonic_clock() = delete;

  using duration = chrono::nanoseconds;

  using time_point = chrono::time_point<monotonic_clock, duration>;

  const static bool is_steady{true};

  static GPCL_DECL time_point now() noexcept;

  static const ::clockid_t clock_id{CLOCK_MONOTONIC};
};

struct realtime_clock
{
  realtime_clock() = delete;

  using duration = chrono::nanoseconds;
  using time_point = chrono::time_point<realtime_clock, duration>;

  const static bool is_steady{false};
  static GPCL_DECL time_point now() noexcept;

  static const ::clockid_t clock_id{CLOCK_MONOTONIC};
};

inline constexpr bool timespec_valid(const struct timespec *t)
{
  return !!t && t->tv_nsec >= 0 && t->tv_nsec < 1'000'000'000;
}

inline constexpr bool timespec_eq(const struct timespec *a,
                                  const struct timespec *b)
{
  return a->tv_sec == b->tv_sec && a->tv_nsec == b->tv_nsec;
}

inline constexpr bool timespec_gt(const struct timespec *a,
                                  const struct timespec *b)
{
  if (a->tv_sec > b->tv_sec)
    return true;
  if (a->tv_sec == b->tv_sec)
    return a->tv_nsec > b->tv_nsec;
  return false;
}

inline constexpr int timespec_compare(const struct timespec *a, const struct timespec *b)
{
  if (!a)
  {
    if (!b)
      return 0;
    return -1;
  }

  if (!b)
    return 1;

  if (a->tv_sec > b->tv_sec)
    return 1;

  if (a->tv_sec < b->tv_sec)
    return -1;

  return (a->tv_nsec > b->tv_nsec) ? 1 :
                                     (a->tv_nsec == b->tv_nsec) ? 0 :
                                                                  -1;
}

inline constexpr bool timespec_is_zero(const struct timespec *t)
{
  return t->tv_sec == 0 && t->tv_nsec == 0;
}

inline constexpr void timespec_add(struct timespec *a, const struct timespec *b)
{
  GPCL_ASSERT(a != b);
  GPCL_ASSERT(timespec_valid(a));
  GPCL_ASSERT(timespec_valid(b));

  a->tv_sec += b->tv_sec;
  a->tv_nsec += b->tv_nsec;

  if (a->tv_nsec >= 1'000'000'000)
  {
    a->tv_sec += 1;
    a->tv_nsec -= 1'000'000'000;
  }
}

inline constexpr void timespec_sub(struct timespec *a, const struct timespec *b)
{
  GPCL_ASSERT(a != b);
  GPCL_ASSERT(timespec_valid(a));
  GPCL_ASSERT(timespec_valid(b));

  a->tv_nsec -= b->tv_nsec;
  a->tv_sec -= b->tv_sec;

  if (a->tv_nsec < 0)
  {
    a->tv_nsec += 1'000'000'000;
    a->tv_sec -= 1;
  }
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_CLOCK_HPP
