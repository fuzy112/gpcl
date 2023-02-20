#include "doctest.h"

#include <gpcl/detail/config.hpp>
#include <gpcl/semaphore.hpp>

#if defined(GPCL_LINUX)

#  include <gpcl/detail/futex_atomic_wait.hpp>
#  include <thread>

using namespace gpcl::detail;

TEST_CASE("futex_atomic_wait_test")
{
  short v = 0;

  bool result = false;

  std::thread t1([&] {
    futex_atomic_wait_on_address(&v, short(0), __ATOMIC_SEQ_CST);

    short r;
    __atomic_load(&v, &r, __ATOMIC_ACQUIRE);
    CHECK(r != 0);
    result = true;
  });

  std::thread t2([&] {
    while (!futex_atomic_wait_state::for_(&v).is_waiting())
      std::this_thread::sleep_for(std::chrono::microseconds(10));

    short newval = 256;
    __atomic_store(&v, &newval, __ATOMIC_RELEASE);
    futex_atomic_wake_by_address(&v, false);
  });

  t1.join();
  t2.join();

  REQUIRE(result);
}

#endif
