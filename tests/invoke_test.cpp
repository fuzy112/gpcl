#include <gpcl/invoke.hpp>

#include "doctest.h"

class MyClass
{
public:
  void f1(int) {}

  int f2(int, int) const { return 100; }

  int x = 42;
};

TEST_CASE("invoke")
{
  MyClass my_object;

  CHECK_NOTHROW(gpcl::invoke(&MyClass::f1, my_object, 111));
  CHECK(100 == gpcl::invoke(&MyClass::f2, std::cref(my_object), 1, 2));
  CHECK(42 == gpcl::invoke(&MyClass::x, &my_object));
}
