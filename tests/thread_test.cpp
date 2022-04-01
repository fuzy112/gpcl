#include <gpcl/thread.hpp>

#include "doctest.h"

TEST_CASE("thread id")
{
  gpcl::thread_id id1, id2;

  gpcl::thread t([&] { id1 = gpcl::this_thread::get_id(); });

  id2 = t.get_id();

  t.join();

  CHECK(id1 == id2);
  CHECK(!!id1);
}

TEST_CASE("thread::hardware_concurrency")
{
  CHECK(gpcl::thread::hardware_concurrency() != 0);
}

#include <gpcl/tss_ptr.hpp>

TEST_CASE("tss_ptr")
{
  static gpcl::tss_ptr<int> tss;

  gpcl::thread t1([&] {
    int i;
    tss = &i;
    CHECK(tss == &i);
  });
  gpcl::thread t2([&] {
    int i;
    tss = &i;
    CHECK(tss == &i);
  });
  t1.join();
  t2.join();

  CHECK(tss == nullptr);
}
