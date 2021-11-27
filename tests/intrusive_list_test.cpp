#include "doctest.h"

#include <gpcl/intrusive_list.hpp>
#include <string>

struct Entry : gpcl::intrusive_list_node<Entry, class tag1>,
               gpcl::intrusive_list_node<Entry, class Tag2>
{
  std::string s;

  explicit Entry(std::string s) : s(s) {}
};

TEST_CASE("intrusive list")
{
  Entry e1{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"};
  gpcl::intrusive_list<Entry, class tag1> str_list;
  gpcl::intrusive_list<Entry, class Tag2> str_list2;

  Entry e2{"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"};
  REQUIRE(str_list.empty());
  REQUIRE(str_list2.empty());

  str_list.push_back(e1);
  REQUIRE(&str_list.front() == &e1);
  REQUIRE(&str_list.back() == &e1);

  str_list2.push_back(e1);
  REQUIRE(&str_list.front() == &e1);
  REQUIRE(&str_list.back() == &e1);

  REQUIRE(&str_list2.front() == &e1);
  REQUIRE(&str_list2.back() == &e1);

  str_list.push_back(e2);
  REQUIRE(&str_list.front() == &e1);
  REQUIRE(&str_list.back() == &e2);

  str_list.pop_back();
  REQUIRE(str_list.is_singular());
}
