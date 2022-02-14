//
// assertion_failure.ipp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP
#define GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP

#include <gpcl/basic_stacktrace.hpp>
#include <gpcl/debugstream.hpp>
#include <gpcl/detail/assertion_failure.hpp>

#include <cstdlib>

namespace gpcl::detail {

void assertion_failure(const char *expr, const char *file,
                       std::uint_least32_t line, const char *func)
{
  thread_local static bool assertion_failed = false;

  if (assertion_failed)
  {
    std::abort();
  }
  assertion_failed = true;

  cdebug() << file
#if defined(GPCL_WINDOWS)
           << '(' << line << ')'
#else
           << ":" << line
#endif
           << ": In function '" << func << "': "
           << "Assertion failed: " << expr << "\n"
#if defined(GPCL_STACKTRACE)
           << "Backtrace:\n"
           << basic_stacktrace<std::allocator<stacktrace_entry>>::current(2)
#endif
           << std::endl;
  std::abort();
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP
