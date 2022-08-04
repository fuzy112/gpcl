#include "doctest.h"

#include <gpcl/clone_ptr.hpp>

namespace {
struct MyClass
{
  int x;

  MyClass* clone() const
  {
    return new MyClass;
  }
};
}

TEST_CASE("clone_ptr")
{
  gpcl::clone_ptr<MyClass> o(new MyClass);
  auto o2 = o;
  auto o3 {o};
}
