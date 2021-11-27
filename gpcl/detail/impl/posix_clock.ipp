//
// posix_clock.ipp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/posix_clock.hpp>
#include <ctime>

#if defined(GPCL_POSIX)
#  include <time.h>

namespace gpcl {
namespace detail {

monotonic_clock::time_point monotonic_clock::now() noexcept
{
  auto ts = timespec{};
  GPCL_VERIFY(0 == clock_gettime(CLOCK_MONOTONIC, &ts));
  return time_point(to_duration(ts));
}

realtime_clock::time_point realtime_clock::now() noexcept
{
  auto ts = timespec{};
  GPCL_VERIFY(0 == clock_gettime(CLOCK_REALTIME, &ts));
  return time_point(to_duration(ts));
}

} // namespace detail
} // namespace gpcl

#endif
