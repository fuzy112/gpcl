#include "doctest.h"

#include <gpcl/atomic.hpp>
#include <gpcl/thread.hpp>

namespace gpcl {

template struct atomic<char>;
template struct atomic<unsigned char>;
template struct atomic<signed char>;
template struct atomic<short>;
template struct atomic<unsigned short>;

} // namespace gpcl

template <typename T>
void test_atomic()
{
  gpcl::atomic<T> ac{1};

  CHECK(ac.load() == 1);
  ac.store(2);
  CHECK(ac.load() == 2);
  ac.store(3);
  CHECK(ac.load() == 3);
  CHECK(++ac == 4);
  CHECK(ac++ == 4);
  CHECK(ac.load() == 5);

  CHECK(ac.exchange(8) == 5);
  CHECK(ac.load() == 8);

  T e = 8;
  CHECK(ac.compare_exchange_strong(e, 9));
  CHECK(ac.load() == 9);
  CHECK(e == 8);

  CHECK_FALSE(ac.compare_exchange_strong(e, 10));
  CHECK(ac.load() == 9);

  ac.store(1);
  CHECK(ac.fetch_add(1) == 1);
  CHECK(ac.load() == 2);

  ac.store(1);
  CHECK(ac++ == 1);
  CHECK(ac.load() == 2);

  ac.store(1);
  CHECK(ac.fetch_sub(1) == 1);
  CHECK(ac.load() == 0);

  ac.store(1);
  CHECK(ac.fetch_and(2) == 1);
  CHECK(ac.load() == 0);

  ac.store(1);
  CHECK(ac.fetch_or(2) == 1);
  CHECK(ac.load() == 3);

  ac.store(1);
  CHECK(ac.fetch_xor(3) == 1);
  CHECK(ac.load() == 2);

  ac.store(1);
  std::vector<gpcl::thread> threads;
  for (int i = 0; i < 10; ++i)
    threads.emplace_back([&] {
      ac.wait(1);
      CHECK(ac.load() != 1);
    });
  ac.store(2);
  ac.notify_all();
  for (auto &&t : threads)
    t.join();
}

TEST_CASE("test atomic specialization")
{
  test_atomic<int64_t>();
  test_atomic<uint64_t>();
  test_atomic<int32_t>();
  test_atomic<uint32_t>();
  test_atomic<int16_t>();
  test_atomic<uint16_t>();
  test_atomic<int8_t>();
  test_atomic<uint8_t>();
  test_atomic<long long>();
  test_atomic<unsigned long long>();
  test_atomic<long>();
  test_atomic<unsigned long>();
  test_atomic<int>();
  test_atomic<unsigned int>();
  test_atomic<short>();
  test_atomic<unsigned short>();
  test_atomic<char>();
  test_atomic<unsigned char>();
  test_atomic<signed char>();
  test_atomic<wchar_t>();
  test_atomic<wint_t>();
  test_atomic<size_t>();
  test_atomic<char16_t>();
  test_atomic<char32_t>();
}

struct S
{
  int a;
  char b;
};

TEST_CASE("test atomic primary template")
{
  gpcl::atomic<bool> ab;
  gpcl::atomic<bool> ab2{true};

  CHECK(ab.load() == false);
  CHECK(ab2.load() == true);

  ab.store(2);
  CHECK(ab.load() == true);

  gpcl::atomic<S> as;
  CHECK(as.load().a == 0);
  CHECK(as.load().b == 0);

  as.store(S{1, 2});
  CHECK(as.load().a == 1);
  CHECK(as.load().b == 2);
}
