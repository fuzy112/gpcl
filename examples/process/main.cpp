#include <gpcl/debugstream.hpp>
//#include <gpcl/impl/win_main.hpp>
#include <gpcl/process.hpp>
#include <gpcl/signal.hpp>
#include <gpcl/stacktrace.hpp>

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  GPCL_TRY
  {
#ifdef GPCL_WINDOWS
    gpcl::dynarray<std::string> args{"notepad.exe"};
#else
    gpcl::dynarray<std::string> args{"ls"};
#endif
    gpcl::process proc(args);

    if (!proc.try_join_for(gpcl::chrono::seconds(5)))
    {
      proc.kill();
      proc.join();
    }

    if (proc.exited())
    {
      gpcl::cdebug() << "Exit code: " << proc.exit_code() << std::endl;
    }
    else if (proc.killed())
    {
      gpcl::cdebug() << "Killed by signal: " << gpcl::signal_name(proc.signal())
                     << std::endl;
    }
  }
  GPCL_CATCH(gpcl::system_error const &e)
  {
    gpcl::cdebug() << e.what() << std::endl;
  }
  GPCL_AND_CATCH(std::exception const &e)
  {
    gpcl::cdebug() << e.what() << std::endl;
  }
  GPCL_CATCH_END
}
