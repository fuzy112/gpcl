#include <gpcl/exception.hpp>
#include <gpcl/service.hpp>
#include <gpcl/getopt.hpp>

#include <iostream>
#include <unistd.h>

class my_service : public gpcl::service
{
  int pipes[2];

public:
  my_service(const char *pidfile)
    : gpcl::service(pidfile)
  {}

  void do_start() override
  {
    GPCL_THROW_LAST_ERROR_IF(pipe(pipes) < 0);

    notify_success();
  }

  void run() override
  {
    char dummy[1];
    GPCL_THROW_LAST_ERROR_IF(read(pipes[0], dummy, sizeof(dummy)) < 0);
  }

  void do_stop() override
  {
    char dummy[1] = {};
    GPCL_THROW_LAST_ERROR_IF(write(pipes[1], dummy, sizeof(dummy)) < 0);
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
  my_service svc("/var/run/test-gpcl.pid");

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
