#include <gpcl/debugstream.hpp>
//#include <gpcl/impl/win_main.hpp>
#include <gpcl/process.hpp>
#include <gpcl/signal.hpp>
#include <gpcl/stacktrace.hpp>
#include <gpcl/thread.hpp>

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  gpcl::dynarray<gpcl::thread> threads;

  for (int i = 0; i < 10; ++i)
    threads.emplace_back([i] {
      GPCL_TRY
      {
#ifdef GPCL_WINDOWS
        gpcl::dynarray<std::string> args{"notepad.exe"};
#else
        gpcl::dynarray<std::string> args{"echo", std::to_string(i)};
#endif
        gpcl::process proc(args);

        if (!proc.try_join_for(gpcl::chrono::seconds(0)))
        {
          proc.terminate();
          proc.join();
        }

        if (proc.exited())
        {
          gpcl::cdebug() << i << " Exit code: " << proc.exit_code()
                         << std::endl;
        }
        else if (proc.killed())
        {
          gpcl::cdebug() << i << " Killed by signal: "
                         << gpcl::signal_name(proc.signal()) << std::endl;
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
    });

  for (auto &t : threads)
    t.join();
}
