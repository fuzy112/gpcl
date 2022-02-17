#include <gpcl/lexical_cast.hpp>
#include <gpcl/variant.hpp>

#include "doctest.h"

using gpcl::lexical_cast;
using gpcl::variant;
using gpcl::visit;

using variant_type = variant<int, double, std::string, char>;

template <typename Target, typename... Types>
Target variant_to(const variant<Types...> &v)
{
  return visit(lexical_cast<Target>, v);
}

TEST_CASE("lexical_cast")
{
  int x = 42;

  CHECK(lexical_cast<std::wstring>(x) == L"42");

  CHECK(lexical_cast<double>("0.5") == 0.5);
  CHECK(lexical_cast<int>("123456", 3) == 123);
  CHECK(lexical_cast<std::string>("123456", 3) == "123");

  variant_type v = 42;
  CHECK(variant_to<int>(v) == 42);
  CHECK(variant_to<float>(v) == 42);
  CHECK(variant_to<std::string>(v) == "42");

#ifndef GPCL_CONFIG_NO_EXCEPTIONS
  CHECK_THROWS(variant_to<char>(v));
#endif
}
