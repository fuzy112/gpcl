#ifndef GPCL_DETAIL_STACKTRACE_ASSERTION_FAILURE_HOOK_HPP
#define GPCL_DETAIL_STACKTRACE_ASSERTION_FAILURE_HOOK_HPP

#include <gpcl/assert.hpp>
#include <gpcl/basic_stacktrace.hpp>
#include <gpcl/detail/assertion_failure.hpp>

#include <iostream>

namespace gpcl::detail {

inline void assertion_failure(const char *expr, const char *file,
                              std::uint_least32_t line, const char *func,
                              assertion_failure_hook_tag)
{
  thread_local static bool assertion_failed = false;

  if (assertion_failed)
  {
    abort();
  }
  assertion_failed = true;

  std::clog << file << ":" << line << ": In function '" << func << "': "
            << "Assertion failed: " << expr << "\n";
  std::clog << "Backtrace:\n";
  std::clog << basic_stacktrace<std::allocator<stacktrace_entry>>::current(2)
            << std::endl;
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_STACKTRACE_ASSERTION_FAILURE_HOOK_HPP
