#include "doctest.h"

#include <gpcl/any.hpp>
#include <gpcl/vector.hpp>
#include <iostream>

TEST_CASE("basic_any")
{
  std::cout << std::boolalpha;

  // basic_any type
  gpcl::basic_any<> a = 1;

#if !defined GPCL_CONFIG_NO_RTTI
  REQUIRE(gpcl::any_cast<int>(a) == 1);
  a = 3.14;
  REQUIRE(gpcl::any_cast<double>(a) == 3.14);

  a = true;
  REQUIRE(gpcl::any_cast<bool>(a));

#if !defined GPCL_CONFIG_NO_EXCEPTIONS
  // bad cast
  a = 1;
  REQUIRE_THROWS(gpcl::any_cast<float>(a));
#endif
#endif
  // has value
  a = 1;
  REQUIRE(a.has_value());

  // reset
  a.reset();
  REQUIRE_FALSE(a.has_value());

#if !defined GPCL_CONFIG_NO_RTTI
  // pointer to contained data
  a = 1;
  int *i = gpcl::any_cast<int>(&a);
  REQUIRE(i);
  REQUIRE(*i == 1);
#endif

  a = 1;
  REQUIRE(1 == *gpcl::any_cast_unchecked<int>(&a));
}

TEST_CASE("make_any")
{
  using gpcl::any_cast;
  auto vec = gpcl::make_any<gpcl::vector<int>>({1, 2, 3});
  REQUIRE(vec.has_value());
#if !defined GPCL_CONFIG_NO_RTTI
  REQUIRE(any_cast<gpcl::vector<int>>(vec) == gpcl::vector<int>{1, 2, 3});
#endif
  REQUIRE(*gpcl::any_cast_unchecked<gpcl::vector<int>>(&vec) ==
          gpcl::vector<int>{1, 2, 3});
}
