//
// clock.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CLOCK_HPP
#define GPCL_CLOCK_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_clock.hpp>
#include <gpcl/detail/win_clock.hpp>

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

#endif
