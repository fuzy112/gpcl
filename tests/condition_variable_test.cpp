#include "doctest.h"

#include <gpcl/detail/futex_condition_variable.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/unique_lock.hpp>
#include <deque>

TEST_CASE("futex_condition_variables")
{
  gpcl::detail::futex_mutex mtx;
  gpcl::detail::futex_condition_variable cond;
  std::deque<int> queue;
  const int end = 1000000;

  gpcl::thread consumer([&] {
    int n = 0;
    while (n != end)
    {
      mtx.lock();
      while (queue.empty())
      {
        cond.wait(mtx);
      }

      n = queue.front();
      queue.pop_front();
      mtx.unlock();
    }
  });

  gpcl::thread producer([&] {
    int n = 0;

    while (n != end + 1)
    {
      gpcl::unique_lock<gpcl::detail::futex_mutex> lock(mtx);
      queue.push_back(n++);
      lock.unlock();
      cond.notify_one();
    }
  });

  consumer.join();
  producer.join();
}

#include <gpcl/detail/posix_condition_variable.hpp>
#include <gpcl/detail/posix_mutex.hpp>

TEST_CASE("posix_condition_variables")
{

  gpcl::detail::posix_normal_mutex mtx;
  gpcl::detail::posix_condition_variable cond;
  std::deque<int> queue;
  const int end = 1000000;

  gpcl::thread consumer([&] {
    int n = 0;
    while (n != end)
    {
      gpcl::unique_lock<gpcl::detail::posix_normal_mutex> lock(mtx);
      while (queue.empty())
      {
        cond.wait(lock);
      }

      n = queue.front();
      queue.pop_front();
    }
  });

  gpcl::thread producer([&] {
    int n = 0;

    while (n != end + 1)
    {
      gpcl::unique_lock<gpcl::detail::posix_normal_mutex> lock(mtx);
      queue.push_back(n++);
      lock.unlock();
      cond.notify_one();
    }
  });

  consumer.join();
  producer.join();
}
