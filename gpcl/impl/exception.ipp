#pragma once

#include <gpcl/debugstream.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/stacktrace.hpp>

namespace gpcl {

#if defined GPCL_NO_EXCEPTIONS
void throw_exception(std::exception const &e)
{
  cdebug() << "Trying to an exception [" << e.what()
           << "], but exception support is disabled.\n"
           << "\nTracing back:\n"
           << stacktrace::current() << "\nTerminating..." << std::endl;
  std::terminate();
}
#endif

} // namespace gpcl
