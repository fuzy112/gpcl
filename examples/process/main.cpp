#include <gpcl/debugstream.hpp>
#include <gpcl/detail/win_process.hpp>
//#include <gpcl/impl/win_main.hpp>
#include <gpcl/stacktrace.hpp>

int main(int argc, char **argv)
{
#if defined(GPCL_WINDOWS)
  GPCL_TRY
  {
    gpcl::detail::win_process proc(
        gpcl::detail::win_process::command_line_tag{}, "notepad.exe");
    if (!proc.try_join_for(gpcl::chrono::seconds(5)))
    {
      // proc.kill();
      // proc.join();
    }
    
    gpcl::cdebug() << "Exit code: " << proc.exit_code() << std::endl;
  }
  GPCL_CATCH(gpcl::system_error const &e)
  {
    gpcl::cdebug() << e.what() << std::endl;
  }
  GPCL_CATCH(std::exception const &e)
  {
    gpcl::cdebug() << e.what() << std::endl;
  }
  GPCL_CATCH_END
#endif
}
