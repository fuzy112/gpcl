#ifndef GPCL_PTHREAD_ERROR_HPP
#define GPCL_PTHREAD_ERROR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/source_location.hpp>

namespace gpcl::detail {

inline void throw_if_pthread_failed(int result, const char *what,
                                    source_location location)
{
  if (result != 0)
  {
    throw_errno(result, what, location);
  }
}

#define GPCL_THROW_IF_PTHREAD_FAILED(...)                                      \
  ::gpcl::detail::throw_if_pthread_failed((__VA_ARGS__),                       \
                                          GPCL_TO_STR(__VA_ARGS__),            \
                                          GPCL_SOURCE_LOCATION_CURRENT_LINE())

} // namespace gpcl::detail

#endif // GPCL_PTHREAD_ERROR_HPP
