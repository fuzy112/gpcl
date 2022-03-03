#include <gpcl/exception.hpp>
#include <gpcl/lock_file.hpp>

#include <fstream>
#include <iostream>

#if defined(GPCL_POSIX)
constexpr auto LOCK_FILE = "/var/tmp/gpcl-example-counter.lock";
constexpr auto STATE_FILE = "/var/tmp/gpcl-example-counter.state";
#else
constexpr auto LOCK_FILE = ".gpcl-example-counter.lock";
constexpr auto STATE_FILE  = ".gpcl-example-counter.state";
#endif

int main()
{
  GPCL_TRY
  {
    gpcl::lock_file lf(LOCK_FILE);
    lf.lock();

    unsigned count = 0;
    {
      std::ifstream ifile;
      ifile.open(STATE_FILE);
      ifile >> count;
      count++;
    }

    {
      std::ofstream ofile;
      ofile.open(STATE_FILE, std::ios_base::trunc);
      ofile << count;
    }
    std::cout << count << std::endl;
    return 0;
  }
  GPCL_CATCH(std::exception & exc)
  {
    std::cerr << gpcl::diagnostic_information(exc) << std::endl;
    return 1;
  }
  GPCL_CATCH_END
}
