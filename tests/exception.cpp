#include <gpcl/exception.hpp>

#include <sstream>

#include "doctest.h"

DOCTEST_REGISTER_EXCEPTION_TRANSLATOR(gpcl::exception const &exc)
{
  std::ostringstream ss;
  ss << gpcl::diagnostic_information(exc);
  return doctest::String(ss.str().c_str());
}
