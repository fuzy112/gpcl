#include <gpcl/debugstream.hpp>
#include <gpcl/string_utils.hpp>

#include "doctest.h"

TEST_CASE("split")
{
  const std::string names{"Tom, Fred, Mike, James"};
  const std::vector<std::string> expected{"Tom", "Fred", "Mike", "James"};

  gpcl::ssplit_results results;

  gpcl::split(names, results, ",", gpcl::split_flag::trim_results);

  CHECK(results.size() == expected.size());
  CHECK(results.position(0) == 0);
  CHECK(results.position(1) == 5);
  for (std::size_t i = 0; i < results.size(); ++i)
  {
    CHECK(results.str(i) == expected[i]);
  }
}

TEST_CASE("split c")
{
  gpcl::csplit_results results;

  gpcl::split("abcbecbeb", results, "b", gpcl::split_flag::skip_empty_string);

  CHECK(results.str(0) == "a");
  CHECK(results.str(1) == "c");
  CHECK(results.str(2) == "ec");
  CHECK(results.str(3) == "e");
  CHECK(results.str(4).empty());
  CHECK(results.str(5).empty());
  CHECK(results.size() == 4);
  CHECK(results.position(0) == 0);
  CHECK(results.position(1) == 2);
  CHECK(results.position(2) == 4);
  CHECK(results.position(3) == 7);
}

