//
// posix_clock.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_CLOCK_HPP
#define GPCL_DETAIL_POSIX_CLOCK_HPP

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

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_CLOCK_HPP
