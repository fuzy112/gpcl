#include "doctest.h"

#include <gpcl/function.hpp>

#include <functional>
#include <iostream>

struct Foo
{
  Foo(int num) : num_(num) {}
  void print_add(int i) const { std::cout << num_ + i << '\n'; }
  int num_;
};

void print_num(int i)
{
  std::cout << i << '\n';
}

struct PrintNum
{
  void operator()(int i) const { std::cout << i << '\n'; }
};

TEST_CASE("function")
{
  // store a free function
  gpcl::function<void(int)> f_display = print_num;
  f_display(-9);

  // store a lambda
  gpcl::function<void()> f_display_42 = []() { print_num(42); };
  f_display_42();

  // store the result of a call to std::bind
  gpcl::function<void()> f_display_31337 = std::bind(print_num, 31337);
  f_display_31337();

  // store a call to a member function
  // gpcl::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
  const Foo foo(314159);
  // f_add_display(foo, 1);
  // f_add_display(314159, 1);

  // store a call to a data member accessor
  // gpcl::function<int(Foo const&)> f_num = &Foo::num_;
  // std::cout << "num_: " << f_num(foo) << '\n';

  // store a call to a member function and object
  using std::placeholders::_1;
  gpcl::function<void(int)> f_add_display2 =
      std::bind(&Foo::print_add, foo, _1);
  f_add_display2(2);

  // store a call to a member function and object ptr
  gpcl::function<void(int)> f_add_display3 =
      std::bind(&Foo::print_add, &foo, _1);
  f_add_display3(3);

  // store a call to a function object
  gpcl::function<void(int)> f_display_obj = PrintNum();
  f_display_obj(18);

  auto factorial = [](int n) {
    // store a lambda object to emulate "recursive lambda"; aware of extra
    // overhead
    gpcl::function<int(int)> fac = [&](int n) {
      return (n < 2) ? 1 : n * fac(n - 1);
    };
    // note that "auto fac = [&](int n){...};" does not work in recursive calls
    return fac(n);
  };
  for (int i{5}; i != 8; ++i)
  {
    factorial(i);
  }
}
