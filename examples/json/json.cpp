#include <gpcl/detail/json.hpp>

using gpcl::detail::json;

int main()
{
  json::value_type v{1, 2, 3, 4};
  std::cout << v.at(0) << std::endl;

  v.at(0) = 3;
  std::cout << v << std::endl;

  for (auto iter = v.begin(); iter != v.end(); ++iter)
  {
    std::cout << *iter << std::endl;
  }

  json::value_type m = json::value_type::object({
      {"1", 1},
      {"2", 2},
      {"3", "3"},
  });
  auto m1 = m;
  m["3"] = 3;
  std::cout << m << std::endl;

  m1.insert(
      {"4", json::value_type::array({1, 2, "3", 4.0, true, false, nullptr})});
  m1.erase("2");

  for (auto iter = m1.begin(); iter != m1.end(); ++iter)
  {
    std::cout << iter.key() << ": " << iter.value() << std::endl;
  }
}
