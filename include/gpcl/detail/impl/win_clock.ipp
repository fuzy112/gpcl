//
// win_clock.ipp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_CLOCK_IPP
#define GPCL_DETAIL_IMPL_WIN_CLOCK_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/win_clock.hpp>

#include <profileapi.h>


namespace gpcl {
namespace detail {

auto system_clock::now() noexcept -> system_clock::time_point
{
  auto cnt = ::LARGE_INTEGER{};
  GPCL_VERIFY(::QueryPerformanceCounter(&cnt));

  auto freq = ::LARGE_INTEGER{};
  GPCL_VERIFY(::QueryPerformanceFrequency(&freq));

  auto sec = cnt.QuadPart / freq.QuadPart;
  auto rem = cnt.QuadPart % freq.QuadPart;

  const auto cnt_per_nsec = freq.QuadPart / 1000000000;
  auto nsec = rem / cnt_per_nsec;
  return time_point(duration(sec * 1000000000 + nsec));
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_CLOCK_IPP
