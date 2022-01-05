#include <gpcl.hpp>

int main()
{
  auto pint = gpcl::make_shared<int>(42);
  {
    gpcl::wosyncstream sout(std::wcout);

    sout << pint << std::endl;
  }
  return 0;
}
