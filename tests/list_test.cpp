#include <gpcl/list.hpp>

#include "doctest.h"

#include <random>

GPCL_MSVC_SUPPRESS_WARNING(26800)

TEST_CASE("list")
{
  gpcl::list<int> il;

  il.push_back(1);
  il.push_back(2);
  il.push_back(3);

  CHECK(il.size() == 3);
  CHECK(il.back() == 3);
  CHECK(il.front() == 1);

  int arr[] = {1, 2, 3};
  CHECK(std::equal(il.begin(), il.end(), std::begin(arr), std::end(arr)));

  gpcl::list<int> il2{1, 2, 3};
  CHECK(*il2.begin() == 1);
  CHECK(*std::next(il2.begin(), 2) == 3);
  il.splice(std::next(il.begin()), std::move(il2), il2.begin(),
            std::next(il2.begin(), 2));

  CHECK(il.size() == 5);
  CHECK(il2.size() == 1);
  CHECK(il2.back() == 3);

  gpcl::list il3 = std::move(il2);
  CHECK(il2.size() == 0);
  CHECK(il3.size() == 1);
  CHECK(il3.back() == 3);

  gpcl::list il4 = il3;
  CHECK(il4 == il3);

  gpcl::list il5 = std::move(il3);
  CHECK(il5 == il4);
  il2 = std::move(il5);
  CHECK(il2 == il4);
  il3 = il2;
  CHECK(il3 == il4);

  gpcl::list<int> il6{1, 2, 3};
  gpcl::list<int> il7{4, 5, 6};
  gpcl::list il8 = il6;
  gpcl::list il9 = il7;
  il6.swap(il7);
  CHECK(il9 == il6);
  CHECK(il7 == il8);

  gpcl::list<int> il10{1, 3, 5, 2, 4, 6};
  il10.sort();
  CHECK(std::is_sorted(il10.begin(), il10.end()));

  std::random_device rd;
  std::mt19937 gen(rd());
  for (int i = 0; i < 100; ++i)
    il10.push_back(gen());
  il10.sort();
  CHECK(std::is_sorted(il10.begin(), il10.end()));
}
