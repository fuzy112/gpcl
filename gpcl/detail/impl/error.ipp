//
// error.ipp
// ~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/error.hpp>
#include <cstdio>  // for fprintf
#include <cstring> // for strerror

#ifdef GPCL_USE_BOOST_SYSTEM_ERROR
#  include <boost/throw_exception.hpp>
#endif

namespace gpcl {
namespace detail {

#if defined(GPCL_POSIX)
void throw_system_error(int err, czstring<> what)
{
  if (err == EINTR)
  {
    GPCL_THROW(interrupted());
  }

  GPCL_THROW(system_error(err, generic_category(), what));
}

#elif defined(GPCL_WINDOWS)
[[noreturn]] void throw_system_error(DWORD err, czstring<> what)
{
  GPCL_THROW(system_error(err, system_category(), what));
}
#endif

void print_error(int err, czstring<> what) noexcept
{
#ifndef GPCL_WINDOWS
  std::fprintf(stderr, "%s: %s", what, std::strerror(err));
#else
  std::fprintf(stderr, "%s: %s", what, generic_category().message(err).c_str());
#endif
}

} // namespace detail
} // namespace gpcl
