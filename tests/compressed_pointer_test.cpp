#include <gpcl/detail/compressed_pointer.hpp>
#include "doctest.h"

TEST_CASE("compressed_pointer")
{

  {
    gpcl::detail::compressed_pointer<int> p;
    int *pi = nullptr;
    CHECK(!p);
    CHECK(p == nullptr);
    CHECK((p == pi));
    CHECK(p.get() == nullptr);
    CHECK(p.flags() == 0);
  }
  {
    int i;
    gpcl::detail::compressed_pointer<int> p(&i);
    int *pi = nullptr;
    CHECK(!!p);
    CHECK(p != nullptr);
    CHECK((p != pi));
    CHECK(p.get() != nullptr);
    CHECK(p.get() == &i);
    CHECK(p.flags() == 0);

    p.flags(1);
    CHECK(p.flags() == 1);
    CHECK(p.get() == &i);
  }
}
