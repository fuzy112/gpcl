#include <gpcl/debugstream.hpp>
//#include <gpcl/impl/win_main.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/process.hpp>
#include <gpcl/signal.hpp>
#include <gpcl/stacktrace.hpp>
#include <gpcl/thread.hpp>

int main(int argc, char **argv) GPCL_TRY
{
  gpcl::dynarray<gpcl::process> processes;
  gpcl::dynarray<char const *> args(std::next(argv), std::next(argv, argc));

  for (int i = 0; i < 100; ++i)
    processes.emplace_back(args);

  for (auto &p : processes)
  {
    p.join();
    if (p.killed())
    {
      std::clog << gpcl::signal_name(p.signal()) << '\n';
    }
  }
}
GPCL_CATCH(std::exception &e)
{
  std::clog << gpcl::diagnostic_information(e) << std::endl;
}
GPCL_CATCH_END
