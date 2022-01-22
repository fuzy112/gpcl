#ifndef GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP
#define GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP

#include <gpcl/assert.hpp>
#include <gpcl/basic_stacktrace.hpp>
#include <gpcl/debugstream.hpp>
#include <gpcl/detail/assertion_failure.hpp>

namespace gpcl::detail {

void assertion_failure(const char *expr, const char *file,
                       std::uint_least32_t line, const char *func)
{
  thread_local static bool assertion_failed = false;

  if (assertion_failed)
  {
    abort();
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
           << "Backtrace:\n"
           << basic_stacktrace<std::allocator<stacktrace_entry>>::current(2)
           << std::endl;
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP
