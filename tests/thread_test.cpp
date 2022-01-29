#include <gpcl/thread.hpp>

#include "doctest.h"

TEST_CASE("thread id")
{
  gpcl::thread_id id1, id2;

  gpcl::thread t([&] {
      id1 = gpcl::this_thread::get_id();
  });

  id2 = t.get_id();

  t.join();

  CHECK(id1 == id2);
  CHECK(!!id1);
}

TEST_CASE("thread::hardware_concurrency")
{
  CHECK(gpcl::thread::hardware_concurrency() != 0);
}

