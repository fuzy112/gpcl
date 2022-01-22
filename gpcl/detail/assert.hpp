#ifndef GPCL_DETAIL_ASSERT_HPP
#define GPCL_DETAIL_ASSERT_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/detail/assertion_failure.hpp>
#include <gpcl/detail/unreachable.hpp>

#include <cassert>
#include <cstdlib>
#include <cstring>

#if !defined(GPCL_ASSERTION_FAILURE_HANDLER)
#  define GPCL_ASSERTION_FAILURE_HANDLER ::gpcl::detail::assertion_failure
#endif

/// \entity GPCL_ASSERT
/// General purpose assertion
#define GPCL_ASSERT(expr)                                                      \
  do                                                                           \
  {                                                                            \
    if (!!(expr))                                                              \
      break;                                                                   \
    GPCL_ASSERTION_FAILURE_HANDLER(#expr, __FILE__, __LINE__, __func__);       \
    std::abort();                                                              \
  } while (false)

#define GPCL_ASSERT_CONST assert

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

#endif // GPCL_DETAIL_ASSERT_HPP
