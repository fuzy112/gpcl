#include <gpcl/getopt.hpp>

inline int verbosity = 0;

constexpr const gpcl::getopt_option options[] = {
    {"help", 'h', gpcl::getopt_no_arg, nullptr, 'h',
     "Print this help message."},
    {"verbose", 'v', gpcl::getopt_optional, nullptr, 'v',
     "Set the verbosity of the program.", "0-5"},
    {},
};

int main(int argc, char **argv)
{
  gpcl::getopt getopt(argc, argv, options);

  int c;
  while ((c = getopt()) != -1)
  {
    switch (c)
    {
    case 'h':
      std::cout << getopt << std::endl;
      break;

    case 'v':
      if (getopt.optarg())
        verbosity = atoi(getopt.optarg());
      else
        verbosity = 5;
      break;

    default:
      std::cerr << "unknown option: \n";
      break;
    }
  }

  std::cout << "Verbosity: " << verbosity << "\n";
}
