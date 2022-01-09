#include <gpcl/meta.hpp>
#include <cxxabi.h>
#include <iostream>

using namespace gpcl;

struct _c {};

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

  using list2 = meta::push_back<list, bool>;
  static_assert(list2::size() == 4);

  using at_3 = meta::at_c<list2, 3>;
  static_assert(std::is_same_v<at_3, bool>);

  using list3 = meta::pop_front<list2>;
  static_assert(list3::size() == 3);

  using list4 = meta::pop_back<list3>;
  static_assert(list4::size() == 2);

  using list5 =
      meta::concat<meta::list<int, char, short>, meta::list<char, wchar_t>,
                   meta::list<bool>, meta::list<>>;
  static_assert(
      std::is_same_v<list5, meta::list<int, char, short, char, wchar_t, bool>>);

  using list6 = meta::as_list<std::tuple<int, float>>;
  static_assert(std::is_same_v<list6, meta::list<int, float>>);

  using tuple1 = meta::as_tuple<list6>;
  static_assert(std::is_same_v<tuple1, std::tuple<int, float>>);

  static_assert(meta::find_type<list6, float>::value == 1);

  using is_same = meta::lambda<class _a, class _b, std::is_same<_a, _b>>;
  static_assert(meta::invoke<is_same, int, int>::value);
  static_assert(!meta::invoke<is_same, int, float>::value);

  using list7 =
      meta::transform<list6, meta::quote_trait<std::add_lvalue_reference>>;
  static_assert(std::is_same_v<list7, meta::list<int &, float &>>);

  using list8 =
      meta::transform<list6,
                      meta::lambda<_c, std::add_lvalue_reference_t<_c>>>;

  meta::list<int &, float &> t = list8{};
  static_assert(std::is_same_v<list8, meta::list<int &, float &>>);
}
