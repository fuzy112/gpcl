#include "doctest.h"

#include <gpcl/once_flag.hpp>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

using namespace gpcl;

#if defined(GPCL_POSIX)

TEST_CASE("once_flag concurrency")
{
  static once_flag once;
  static int count = 0;

  std::vector<std::thread> threads;

  for (int i = 0; i < 4; ++i)
  {
    threads.emplace_back([&]() {
      for (int j = 0; j < 100; ++j)
      {
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        call_once(once, [&] { ++count; });
      }
    });
  }

  for (auto &t : threads)
  {
    t.join();
  }

  REQUIRE(count == 1);
}

TEST_CASE("once_flag recursive")
{
  once_flag once1;
  once_flag once2;

  int var1 = 0;
  int var2 = 0;

  auto init1 = [&]() { var1++; };

  auto init2 = [&]() {
    call_once(once1, init1);
    var2 += var1;
  };

  call_once(once2, init2);
  call_once(once2, init2);
  call_once(once1, init1);

  REQUIRE(var1 == 1);
  REQUIRE(var2 == 1);
}

#  if __cpp_exceptions
TEST_CASE("once_flag exception")
{
  once_flag once;
  int init_count = 0;
  int value = 0;

  auto init = [&] {
    if (init_count++ == 0)
    {
      throw 1;
    }
    value++;
  };

  REQUIRE_THROWS(call_once(once, init));
  REQUIRE(value == 0);
  REQUIRE(init_count == 1);
  REQUIRE_NOTHROW(call_once(once, init));
  REQUIRE(value == 1);
}
#  endif

#endif
