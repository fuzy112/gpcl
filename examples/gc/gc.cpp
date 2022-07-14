#include <gpcl/gc.hpp>

#include <iostream>

int *fib(int *n)
{
  if (*n < 2)
    return n;
  return new (gpcl::gc_tag) int(*fib(new (gpcl::gc_tag) int(*n - 1)) +
                                *fib(new (gpcl::gc_tag) int(*n - 2)));
}

int main()
{
  gpcl::auto_gc();

  auto *n = fib(new (gpcl::gc_tag) int(30));

  // gpcl::stop_auto_gc();
  gpcl::gc();

  std::cout << *n << '\n';
}
