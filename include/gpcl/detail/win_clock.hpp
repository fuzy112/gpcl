//
// win_clock.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_CLOCK_HPP
#define GPCL_DETAIL_WIN_CLOCK_HPP

#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/config.hpp>

namespace gpcl {
namespace detail {

// implemented in terms of QPC
class steady_clock
{
public:
  using duration = chrono::nanoseconds;
  using time_point = chrono::time_point<steady_clock, duration>;
  static const bool is_steady{true};

  // not implemented
  static GPCL_DECL auto now() noexcept -> time_point;
};

// implemented in terms of GetSystemTimePreciseAsFileTime
class system_clock
{
public:
  using duration = chrono::nanoseconds;
  using time_point = chrono::time_point<system_clock, duration>;
  static const bool is_steady{false};

  // not implemented
  static GPCL_DECL auto now() noexcept -> time_point;
};

} // namespace detail
} // namespace gpcl

#endif
