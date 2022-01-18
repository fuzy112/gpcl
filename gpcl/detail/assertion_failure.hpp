#ifndef GPCL_DETAIL_ASSERTION_FAILURE_HPP
#define GPCL_DETAIL_ASSERTION_FAILURE_HPP

#include <gpcl/detail/config.hpp>

#include <cstdint>

namespace gpcl::detail {

[[noreturn]] GPCL_DECL void assertion_failure(const char *expr,
                                              const char *file,
                                              std::uint_least32_t line,
                                              const char *func);
} // namespace gpcl::detail

#endif // GPCL_DETAIL_ASSERTION_FAILURE_HPP
