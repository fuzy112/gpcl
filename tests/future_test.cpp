#include "doctest.h"

#include <gpcl/future.hpp>

TEST_CASE("future")
{
  auto fut = gpcl::async([] { return 1; });
  for (int i = 0; i < 1000; ++i)
  {
    fut = fut.then([](int a) { return a + 1; });
  }
  CHECK(fut.get() == 1001);
}
