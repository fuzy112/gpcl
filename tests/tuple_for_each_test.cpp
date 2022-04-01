#include "doctest.h"

#include <gpcl/detail/tuple_for_each.hpp>

TEST_CASE("tuple_for_each")
{
  std::tuple<> empty;
  std::tuple<int> single;
  std::tuple<char, float> double_;
  std::tuple<int, float, double> triple;

  using ::gpcl::detail::tuple_for_each;

  tuple_for_each(empty, [] (auto v) {
    FAIL("unreachable");
  });

  int n = 0;
  tuple_for_each(single, [&](auto v) {
    ++n;
  });
  CHECK(n == 1);

  n = 0;
  tuple_for_each(double_, [&](auto v) {
    ++n;
  });
  CHECK(n == 2);

  n = 0;
  tuple_for_each(triple, [&](auto v) {
    ++n;
  });
  CHECK(n == 3);
}
