#ifndef GPCL_DETAIL_ASSERTION_FAILURE_HPP
#define GPCL_DETAIL_ASSERTION_FAILURE_HPP

#include <gpcl/detail/config.hpp>

#include <cstdint>
#include <cstdlib>

namespace gpcl::detail {

inline void assertion_failure_hook(...)
{
  std::abort();
}

[[noreturn]] inline void assertion_failure(const char *expr, const char *file,
                                           std::uint_least32_t line,
                                           const char *func)
{
  assertion_failure_hook(expr, file, line, func);
  abort();
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ASSERTION_FAILURE_HPP
