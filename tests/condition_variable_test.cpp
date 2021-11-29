#include "doctest.h"

#include <gpcl/condition_variable.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/unique_lock.hpp>
#include <deque>

TEST_CASE("condition_variable")
{
  gpcl::mutex mtx;
  gpcl::condition_variable cond;
  std::deque<int> queue;
  const int end = 1000;

  gpcl::thread consumer([&] {
    int n = 0;
    while (n != end)
    {
      gpcl::unique_lock<gpcl::mutex> lock(mtx);
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
      gpcl::unique_lock<gpcl::mutex> lock(mtx);
      queue.push_back(n++);
      lock.unlock();
      cond.notify_one();
    }
  });

  consumer.join();
  producer.join();
}
