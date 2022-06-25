#include <gpcl/scoped_array.hpp>

#include "doctest.h"

TEST_CASE("scoped_array")
{
  gpcl::scoped_array<int> arr(new int[4]{1, 2, 3, 4});

  CHECK(arr[0] == 1);
  CHECK(arr[1] == 2);
  CHECK(arr[2] == 3);
  CHECK(arr[3] == 4);
  
  arr[0] = 5;
  arr[1] = 6;
  arr[2] = 7;
  arr[3] = 8;
  CHECK(arr[0] == 5);
  CHECK(arr[1] == 6);
  CHECK(arr[2] == 7);
  CHECK(arr[3] == 8);
}
