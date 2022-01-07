#include <gpcl/lexical_cast.hpp>
#include <gpcl/variant.hpp>
#include <iostream>

using gpcl::lexical_cast;
using gpcl::variant;
using gpcl::visit;

using variant_type = variant<int, double, std::string, char>;

template <typename Target, typename... Types>
Target variant_to(const variant<Types...> &v)
{
  return visit(lexical_cast<Target>, v);
}

int main()
{
  int x = 42;

  std::wcout << lexical_cast<std::wstring>(x) << std::endl;

  std::cout << lexical_cast<double>("0.5") << std::endl;

  variant_type v = "42";
  std::cout << variant_to<int>(v) << std::endl;
  std::cout << variant_to<float>(v) << std::endl;
  std::cout << variant_to<std::string>(v) << std::endl;

#if !defined GPCL_NO_EXCEPTIONS
  // This will crash without exception support.
  GPCL_TRY 
  { 
    std::cout << variant_to<char>(v) << std::endl; 
  }
  GPCL_CATCH(gpcl::bad_lexical_cast const &)
  {
    std::cout << "bad_lexical_cast\n";
  }
  GPCL_CATCH_END
#endif

  std::cout << lexical_cast<int>("123456", 3) << std::endl;

  std::cout << lexical_cast<std::string>("123456", 3) << std::endl;
}
