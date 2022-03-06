#include <gpcl/exception.hpp>
#include <gpcl/service.hpp>
#include <gpcl/getopt.hpp>

#include <iostream>
#include <unistd.h>

class my_service : public gpcl::service
{
public:
  my_service(const char *pidfile)
    : gpcl::service(pidfile)
  {}

  void run() override
  {
    while (!is_stopped())
    {
      sleep(1);
    }
  }
};

void print_usage(std::ostream &out)
{
  out << "service start|stop|restart|help\n";
}

int main(int argc, const char **argv) GPCL_TRY
{
  if (argc != 2)
  {
    print_usage(std::cerr);
    return 1;
  }
  my_service svc("/home/vex/test-gpcl.pid");

  if (argv[1] == std::string_view("start"))
    svc.start();
  else if (argv[1] == std::string_view("stop"))
    svc.stop();
  else if (argv[1] == std::string_view("restart"))
    svc.restart();
  else if (argv[1] == std::string_view("help"))
    print_usage(std::cout);
  else
    { print_usage(std::cerr); return 1; }

  return 0;
}
GPCL_CATCH(std::exception &exc)
{
  std::clog << gpcl::diagnostic_information(exc) << std::endl;
}
GPCL_CATCH_END
