#ifndef GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP
#define GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP

#include <gpcl/basic_stacktrace.hpp>
#include <gpcl/detail/assertion_failure.hpp>

#include <iostream>

namespace gpcl::detail {

[[noreturn]] void assertion_failure(const char *expr, const char *file,
                                    std::uint_least32_t line, const char *func)
{
  std::clog << file << ":" << line << ": In function '" << func << "': "
            << "Assertion failed: " << expr << "\n";
  std::clog << "Backtrace:\n";
  std::clog << basic_stacktrace<std::allocator<stacktrace_entry>>::current()
            << std::endl;
  std::abort();
}

} // namespace gpcl::detali

#endif // GPCL_DETAIL_IMPL_ASSERTION_FAILURE_IPP
