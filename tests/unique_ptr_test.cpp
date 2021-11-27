#include "doctest.h"

#include <gpcl/unique_ptr.hpp>

TEST_CASE("unique_ptr")
{
  gpcl::unique_ptr<int> ip(new int(42));

  REQUIRE(ip);
  REQUIRE(*ip == 42);
  ip.reset();
  REQUIRE(!ip);

  auto ip2 = std::move(ip);
  ip = std::move(ip2);

  auto p3 = gpcl::wrap_unique(new double(42.0));
}
