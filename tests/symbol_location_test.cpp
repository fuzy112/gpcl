#include <gpcl/symbol_location.hpp>

#include "doctest.h"

#include <iostream>

TEST_CASE("symbol location")
{
  static int x = 42;

  std::string loc = gpcl::symbol_location(x);

  CHECK(loc.find("tests") != std::string::npos);
}