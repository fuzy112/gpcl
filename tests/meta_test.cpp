#include <gpcl/meta.hpp>
#include <cxxabi.h>
#include <iostream>

using namespace gpcl;

int main()
{
  using my_list = meta::list<int, double, char>;
  static_assert(my_list::size() == 3);

  using list = meta::list<int, double, float>;
  static_assert(list::size() == 3, "");
  using front = meta::front<list>;
  static_assert(std::is_same<front, int>{}, "");
  using back = meta::back<list>;
  static_assert(std::is_same<back, float>{}, "");
  using at_1 = meta::at_c<list, 1>;
  static_assert(std::is_same<at_1, double>{}, "");
}
