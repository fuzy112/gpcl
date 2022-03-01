#include <gpcl/debugstream.hpp>
//#include <gpcl/impl/win_main.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/process.hpp>
#include <gpcl/signal.hpp>
#include <gpcl/stacktrace.hpp>
#include <gpcl/thread.hpp>

int main(int argc, char **argv)
{
  gpcl::dynarray<gpcl::thread> threads;

  for (int i = 0; i < 100; ++i)
    threads.emplace_back([=] {
      GPCL_TRY
      {
        gpcl::dynarray<char const *> args(std::next(argv),
                                          std::next(argv, argc));
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
      GPCL_CATCH(std::exception const &e)
      {
        gpcl::cdebug() << e.what() << std::endl;
      }
      GPCL_AND_CATCH(...) { gpcl::cdebug() << "UNknown exception\n"; }
      GPCL_CATCH_END
    });

  for (auto &t : threads)
    t.join();
}
