#ifndef GPCL_SERVICE_HPP
#define GPCL_SERVICE_HPP

#include <gpcl/detail/config.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/posix_service.hpp>
#endif

namespace gpcl {

#if defined GPCL_POSIX
using service = detail::posix_service;
#endif

} // namespace gpcl

#endif // GPCL_SERVICE_HPP
