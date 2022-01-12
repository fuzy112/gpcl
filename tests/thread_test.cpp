#include <gpcl/thread.hpp>

#include "doctest.h"

TEST_CASE("thread id")
{
  gpcl::thread_id id1, id2;

  gpcl::thread t([&] {
      id1 = gpcl::this_thread::id();
  });

  id2 = t.id();

  t.join();

  CHECK(id1 == id2);
  CHECK(!!id1);
}
