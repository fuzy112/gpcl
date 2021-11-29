#include "doctest.h"

#include <gpcl/pid_file.hpp>
#include <gpcl/once_flag.hpp>

static gpcl::once_flag once;

#if defined(GPCL_POSIX)
TEST_CASE("pid_file")
{
  gpcl::call_once(once, [] {
    gpcl::pid_file lock("hello.lock");
  });
}
#endif
