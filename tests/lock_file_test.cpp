#include "doctest.h"

#include <gpcl/lock_file.hpp>
#include <gpcl/thread.hpp>

#if 0
TEST_CASE("file lock")
{
  gpcl::lock_file lock_file("test.lock");
  CHECK_FALSE(lock_file.owns_lock());

  gpcl::thread(
      [&]
      {
        gpcl::lock_file lf2("test.lock");
        CHECK(lf2.try_lock());
      });
  CHECK_FALSE(lock_file.owns_lock());

  lock_file.lock();
  CHECK(lock_file.owns_lock());

  gpcl::thread(
      [&]
      {
        gpcl::lock_file lf2("test.lock");
        CHECK_FALSE(lf2.try_lock());
      });
  CHECK(lock_file.owns_lock());

  lock_file.unlock();
  CHECK_FALSE(lock_file.owns_lock());

  lock_file.lock();
  CHECK(lock_file.owns_lock());
}
#endif
