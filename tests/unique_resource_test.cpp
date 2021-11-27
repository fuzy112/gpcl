#include "doctest.h"

#include <gpcl/unique_resource.hpp>

#include <functional>

struct my_deleter
{
  int resource;
  bool &invoked;

  void operator()(int r)
  {
    REQUIRE(!invoked);
    REQUIRE(r == resource);
    invoked = true;
  }
};

TEST_CASE("unique_resource")
{
  int r = 100;
  bool invoked = false;
  {
    gpcl::unique_resource<int, std::function<void(int)>> resource(
        r, my_deleter{r, invoked});

    gpcl::unique_resource<int, std::function<void(int)>> resource2(
        std::move(resource));
  }

  REQUIRE(invoked);

  invoked = false;
  {
    gpcl::unique_resource<int, my_deleter> resource(r, my_deleter{r, invoked});

    gpcl::unique_resource<int, my_deleter> resource2(std::move(resource));
  }
  REQUIRE(invoked);
}
