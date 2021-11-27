#include "doctest.h"

#include <gpcl/pid_file.hpp>

#if defined(GPCL_POSIX)
TEST_CASE("pid_file")
{
  gpcl::pid_file lock("hello.lock");
}
#endif
