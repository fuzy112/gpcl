//
// assert.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ASSERT_HPP
#define GPCL_ASSERT_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unreachable.hpp>

#include <cstring>

#ifdef DOCTEST_REQUIRE
#  define GPCL_ASSERT DOCTEST_REQUIRE
#elif defined GPCL_USE_BOOST_ASSERT
#  include <boost/assert.hpp>
#  define GPCL_ASSERT BOOST_ASSERT
#else

#  ifndef GPCL_WINDOWS
#    include <cassert>
#    define GPCL_ASSERT assert
#  else
#    include <crtdbg.h>
#    define GPCL_ASSERT _ASSERT
#  endif

#endif

#include <cassert>
#define GPCL_ASSERT_CONST assert

/// \entity GPCL_ASSERT
/// General purpose assertion

/// Assertion used to verify post-conditions
#define GPCL_VERIFY(...)                                                       \
  do                                                                           \
  {                                                                            \
    auto _ = static_cast<bool>(__VA_ARGS__);                                   \
    GPCL_ASSERT_CONST(_ &&#__VA_ARGS__);                                       \
    (void)_;                                                                   \
  } while (false)

#define GPCL_VERIFY_(expected, expr) GPCL_VERIFY(expected == (expr))

#define GPCL_VERIFY_0(expr) GPCL_VERIFY_(0, expr)

#define GPCL_VERIFY_FALSE(expr) GPCL_VERIFY_(false, expr)

/// Used to assert that the line cannot be reached
#define GPCL_UNREACHABLE(msg)                                                  \
  ::gpcl::detail::unreachable_internal(msg, __FILE__, __LINE__)

/// Indicates that the function is not implemented yet
#define GPCL_UNIMPLEMENTED() GPCL_UNREACHABLE("unimplemented")

#if defined(_MSC_VER)
#  define GPCL_FATAL(errno)                                                    \
    GPCL_UNREACHABLE(system_category().message(errno).c_str())
#else
#  define GPCL_FATAL(errno) GPCL_UNREACHABLE(std::strerror(errno))
#endif

#endif
