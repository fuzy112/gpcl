#include <gpcl/bind_front.hpp>

#include "doctest.h"

TEST_CASE("bind_front")
{
  auto f = gpcl::bind_front([](int a, int b) {
    return a + b;
  }, 42);

  CHECK(f(1) == 43);
  CHECK(f(42) == 84);
}
