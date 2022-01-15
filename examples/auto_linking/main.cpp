#include <gpcl.hpp>

struct MyObject
{
  MyObject()
  {
    gpcl::osyncstream sout(std::cout);
    sout << "MyObject constructed\n";
    sout << gpcl::stacktrace::current() << std::endl;
  }
  ~MyObject()
  {
    gpcl::osyncstream sout(std::cout);
    sout << "MyObject destructed\n";
    sout << gpcl::stacktrace::current() << std::endl;
  }
};

int main()
{
  auto pint = gpcl::make_shared<MyObject>();
  {
    gpcl::wosyncstream sout(std::wcout);

    sout << pint << std::endl;
  }
  return 0;
}
