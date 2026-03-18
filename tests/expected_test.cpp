//
// expected_test.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "doctest.h"

#include <memory>
#include <new>
#include <string>
#include <type_traits>
#include <vector>

#include <gpcl/expected.hpp>

struct takes_init_and_variadic
{
  std::vector<int> v;
  std::tuple<int, int> t;
  template <class... Args>
  takes_init_and_variadic(std::initializer_list<int> l, Args &&...args)
      : v(l), t(std::forward<Args>(args)...)
  {
  }
};

TEST_CASE("expected constructors")
{
  {
    gpcl::expected<int, int> e;
    REQUIRE(e);
    REQUIRE(e == 0);
  }

  {
    gpcl::expected<int, int> e = gpcl::make_unexpected(0);
    REQUIRE(!e);
    REQUIRE(e.error() == 0);
  }

  {
    gpcl::expected<int, int> e(gpcl::unexpect, 0);
    REQUIRE(!e);
    REQUIRE(e.error() == 0);
  }

  {
    gpcl::expected<int, int> e(gpcl::in_place, 42);
    REQUIRE(e);
    REQUIRE(e == 42);
  }

  {
    gpcl::expected<std::vector<int>, int> e(gpcl::in_place, {0, 1});
    REQUIRE(e);
    REQUIRE((*e)[0] == 0);
    REQUIRE((*e)[1] == 1);
  }

  {
    gpcl::expected<std::tuple<int, int>, int> e(gpcl::in_place, 0, 1);
    REQUIRE(e);
    REQUIRE(std::get<0>(*e) == 0);
    REQUIRE(std::get<1>(*e) == 1);
  }

  {
    gpcl::expected<takes_init_and_variadic, int> e(gpcl::in_place, {0, 1}, 2,
                                                   3);
    REQUIRE(e);
    REQUIRE(e->v[0] == 0);
    REQUIRE(e->v[1] == 1);
    REQUIRE(std::get<0>(e->t) == 2);
    REQUIRE(std::get<1>(e->t) == 3);
  }

  {
    gpcl::expected<int, int> e;
    REQUIRE(std::is_default_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_constructible<decltype(e)>::value);
    REQUIRE(std::is_move_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_assignable<decltype(e)>::value);
    REQUIRE(std::is_move_assignable<decltype(e)>::value);
    REQUIRE(std::is_trivially_copy_constructible<decltype(e)>::value);
    REQUIRE(std::is_trivially_copy_assignable<decltype(e)>::value);
    REQUIRE(std::is_trivially_move_constructible<decltype(e)>::value);
    REQUIRE(std::is_trivially_move_assignable<decltype(e)>::value);
  }

  {
    gpcl::expected<int, std::string> e;
    REQUIRE(std::is_default_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_constructible<decltype(e)>::value);
    REQUIRE(std::is_move_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_assignable<decltype(e)>::value);
    REQUIRE(std::is_move_assignable<decltype(e)>::value);
    REQUIRE(!std::is_trivially_copy_constructible<decltype(e)>::value);
    REQUIRE(!std::is_trivially_copy_assignable<decltype(e)>::value);
    REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
    REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
  }

  {
    gpcl::expected<std::string, int> e;
    REQUIRE(std::is_default_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_constructible<decltype(e)>::value);
    REQUIRE(std::is_move_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_assignable<decltype(e)>::value);
    REQUIRE(std::is_move_assignable<decltype(e)>::value);
    REQUIRE(!std::is_trivially_copy_constructible<decltype(e)>::value);
    REQUIRE(!std::is_trivially_copy_assignable<decltype(e)>::value);
    REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
    REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
  }

  {
    gpcl::expected<std::string, std::string> e;
    REQUIRE(std::is_default_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_constructible<decltype(e)>::value);
    REQUIRE(std::is_move_constructible<decltype(e)>::value);
    REQUIRE(std::is_copy_assignable<decltype(e)>::value);
    REQUIRE(std::is_move_assignable<decltype(e)>::value);
    REQUIRE(!std::is_trivially_copy_constructible<decltype(e)>::value);
    REQUIRE(!std::is_trivially_copy_assignable<decltype(e)>::value);
    REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
    REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
  }

  {
    gpcl::expected<void, int> e;
    REQUIRE(e);
  }

  {
    gpcl::expected<void, int> e(gpcl::unexpect, 42);
    REQUIRE(!e);
    REQUIRE(e.error() == 42);
  }
}

TEST_CASE("expected simple assignment")
{
  gpcl::expected<int, int> e1 = 42;
  gpcl::expected<int, int> e2 = 17;
  gpcl::expected<int, int> e3 = 21;
  gpcl::expected<int, int> e4 = gpcl::make_unexpected(42);
  gpcl::expected<int, int> e5 = gpcl::make_unexpected(17);
  gpcl::expected<int, int> e6 = gpcl::make_unexpected(21);

  e1 = e2;
  REQUIRE(e1);
  REQUIRE(*e1 == 17);
  REQUIRE(e2);
  REQUIRE(*e2 == 17);

  e1 = std::move(e2);
  REQUIRE(e1);
  REQUIRE(*e1 == 17);
  REQUIRE(e2);
  REQUIRE(*e2 == 17);

  e1 = 42;
  REQUIRE(e1);
  REQUIRE(*e1 == 42);

  auto unex = gpcl::make_unexpected(12);
  e1 = unex;
  REQUIRE(!e1);
  REQUIRE(e1.error() == 12);

  e1 = gpcl::make_unexpected(42);
  REQUIRE(!e1);
  REQUIRE(e1.error() == 42);

  e1 = e3;
  REQUIRE(e1);
  REQUIRE(*e1 == 21);

  e4 = e5;
  REQUIRE(!e4);
  REQUIRE(e4.error() == 17);

  e4 = std::move(e6);
  REQUIRE(!e4);
  REQUIRE(e4.error() == 21);

  e4 = e1;
  REQUIRE(e4);
  REQUIRE(*e4 == 21);
}

TEST_CASE("expected assignment deletion")
{
  struct has_all
  {
    has_all() = default;
    has_all(const has_all &) = default;
    has_all(has_all &&) noexcept = default;
    has_all &operator=(const has_all &) = default;
  };

  gpcl::expected<has_all, has_all> e1 = {};
  gpcl::expected<has_all, has_all> e2 = {};
  e1 = e2;

  struct except_move
  {
    except_move() = default;
    except_move(const except_move &) = default;
    except_move(except_move &&) noexcept(false){};
    except_move &operator=(const except_move &) = default;
  };
  gpcl::expected<except_move, except_move> e3 = {};
  gpcl::expected<except_move, except_move> e4 = {};
  // e3 = e4;// should not compile

  (void)e3;
  (void)e4;
}

TEST_CASE("expected emplace")
{
  {
    gpcl::expected<std::unique_ptr<int>, int> e;
    e.emplace(new int{42});
    REQUIRE(e);
    REQUIRE(**e == 42);
  }

  {
    gpcl::expected<std::vector<int>, int> e;
    e.emplace({0, 1});
    REQUIRE(e);
    REQUIRE((*e)[0] == 0);
    REQUIRE((*e)[1] == 1);
  }

  {
    gpcl::expected<std::tuple<int, int>, int> e;
    e.emplace(2, 3);
    REQUIRE(e);
    REQUIRE(std::get<0>(*e) == 2);
    REQUIRE(std::get<1>(*e) == 3);
  }

  {
    gpcl::expected<takes_init_and_variadic, int> e = gpcl::make_unexpected(0);
    e.emplace({0, 1}, 2, 3);
    REQUIRE(e);
    REQUIRE(e->v[0] == 0);
    REQUIRE(e->v[1] == 1);
    REQUIRE(std::get<0>(e->t) == 2);
    REQUIRE(std::get<1>(e->t) == 3);
  }
}

struct move_detector
{
  move_detector() = default;
  move_detector(move_detector &&rhs) noexcept { rhs.been_moved = true; }
  bool been_moved = false;
};

TEST_CASE("expected observers")
{
  gpcl::expected<int, int> o1 = 42;
  gpcl::expected<int, int> o2{gpcl::unexpect, 0};
  const gpcl::expected<int, int> o3 = 42;

  REQUIRE(*o1 == 42);
  REQUIRE(*o1 == o1.value());
  REQUIRE(o2.value_or(42) == 42);
  REQUIRE(o2.error() == 0);
  REQUIRE(o3.value() == 42);
  auto success = std::is_same<decltype(o1.value()), int &>::value;
  REQUIRE(success);
  success = std::is_same<decltype(o3.value()), const int &>::value;
  REQUIRE(success);
  success = std::is_same<decltype(std::move(o1).value()), int &&>::value;
  REQUIRE(success);

  success = std::is_same<decltype(std::move(o3).value()), const int &&>::value;
  REQUIRE(success);

  gpcl::expected<move_detector, int> o4{gpcl::in_place};
  move_detector o5 = std::move(o4).value();
  REQUIRE(o4->been_moved);
  REQUIRE(!o5.been_moved);
}

gpcl::expected<int, int> return_success()
{
  return 1;
}

gpcl::expected<int, int> return_failure()
{
  return gpcl::make_unexpected(0);
}

template <typename Func>
gpcl::expected<void, int> test_try(Func &&func)
{
  GPCL_EXPECTED_TRY(re, func());
  (void)re;
  return {};
}

// #ifdef __GNUC__
// template <typename Func>
// gpcl::expected<void, int> test_try_return(Func &&func)
// {
//   auto e = GPCL_EXPECTED_TRY_RETURN(func());
//   (void)e;
//   return {};
// }
// #endif
//
// TEST_CASE("expected extension")
// {
//   CHECK(test_try(return_success));
//   CHECK(!test_try(return_failure));
//
// #ifdef __GNUC__
//   CHECK(test_try_return(return_success));
//   CHECK(!test_try_return(return_failure));
// #endif
// }
