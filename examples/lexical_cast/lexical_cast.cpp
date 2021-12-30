#include <gpcl/lexical_cast.hpp>
#include <iostream>

using gpcl::lexical_cast;

int main()
{
  int x = 42;

  std::wcout << lexical_cast<std::wstring>(x) << std::endl;

  std::cout << lexical_cast<double>("0.5") << std::endl;
}