//
// clock.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CLOCK_HPP
#define GPCL_CLOCK_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/chrono.hpp>

#if defined(GPCL_POSIX)
#  define GPCL_CLOCK
#  include <gpcl/detail/posix_clock.hpp>
#elif defined(GPCL_WINDOWS)
#  define GPCL_CLOCK
#  include <gpcl/detail/win_clock.hpp>
#endif

namespace gpcl {

namespace chrono = detail::chrono;

#if defined(GPCL_POSIX)
using steady_clock = gpcl::detail::monotonic_clock;

using system_clock = gpcl::detail::realtime_clock;

#elif defined(GPCL_WINDOWS)
using system_clock = detail::system_clock;
using steady_clock = detail::steady_clock;

#endif

} // namespace gpcl

#endif // GPCL_CLOCK_HPP
