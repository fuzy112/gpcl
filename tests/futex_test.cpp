#include "doctest.h"

#include "gpcl/detail/config.hpp"

#if defined(GPCL_LINUX)
#  include <gpcl/detail/futex_mutex.hpp>

TEST_CASE("futex")
{
  gpcl::detail::futex_mutex mtx;

  mtx.lock();
  mtx.unlock();
}

#endif