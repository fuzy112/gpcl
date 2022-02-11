#define GPCL_DEBUG 1
#include <gpcl/array.hpp>

#include "doctest.h"

TEST_CASE("array")
{
  gpcl::array<int> arr(100);
  auto arr2 = arr;
  CHECK(arr2 == arr);
  gpcl::array arr3 = {1, 2, 3, 4};
  arr2 = arr;
  CHECK(arr2 == arr);
  arr = arr3;
  CHECK(arr == arr3);
}
