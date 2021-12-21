#include "doctest.h"

#include <gpcl/mutex.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/unique_lock.hpp>
#include <gpcl/vector.hpp>

const int end = 100;
const int nthreads = 4;

TEST_CASE("mutex")
{
  gpcl::mutex mtx;
  int n = 0;

  auto f1 = [&] {
    for (;;)
    {
      gpcl::unique_lock<gpcl::mutex> lock(mtx);
      if (n == end)
        break;

      n += 1;
    }
  };

  gpcl::vector<gpcl::thread> threads;
  for (int i = 0; i < nthreads; ++i)
    threads.emplace_back(f1);

  for (auto &t : threads)
    t.join();
}

static gpcl::mutex mtx1;
static gpcl::mutex mtx2;

const int i = [] {
  assert(mtx1.native_handle());
  assert(mtx2.native_handle());
  return 0;
}();