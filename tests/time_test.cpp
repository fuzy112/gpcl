#include "doctest.h"

#include <gpcl/time.hpp>

#if !defined(GPCL_CONFIG_NO_EXCEPTIONS)
TEST_CASE("test duration")
{
  CHECK_THROWS(gpcl::duration::max.checked_add(gpcl::duration::second));
}
#endif
