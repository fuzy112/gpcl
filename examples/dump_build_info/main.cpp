#include <gpcl/debugstream.hpp>
#include <gpcl/dump_build_info.hpp>
#include <gpcl/getopt.hpp>

#include <iostream>

constexpr const gpcl::getopt_option options[] = {
    {"help", 'h', gpcl::getopt_no_arg, nullptr, 'h',
     "Print this help message."},
    {"stdout", 'o', gpcl::getopt_no_arg, nullptr, 'o', "Output to stdout only"},
};

int main(int argc, char **argv)
{
  std::cout.sync_with_stdio(false);
  auto cdbg = gpcl::cdebug();
  std::ostream *out = &cdbg;
  gpcl::getopt getopt(argc, argv, options);

  int c;
  while ((c = getopt()) != -1)
  {
    switch (c)
    {
    case 'h':
      std::cout << getopt << std::endl;
      return 0;

    case 'o':
      out = &std::cout;
      break;

    default:
      std::cerr << getopt << std::endl;
      return 1;
    }
  }

  gpcl::dump_build_info(*out);
  
  return 0;
}
