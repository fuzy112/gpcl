#include <gpcl/exception.hpp>

#include "doctest.h"

DOCTEST_REGISTER_EXCEPTION_TRANSLATOR(gpcl::exception const &exc)
{
  return gpcl::diagnostic_information(exc).c_str();
}