#ifndef GPCL_DETAIL_WIN_PROCESS_HPP
#define GPCL_DETAIL_WIN_PROCESS_HPP

#include <gpcl/clock.hpp>
#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/narrow_cast.hpp>
#include <gpcl/noncopyable.hpp>

#include <processthreadsapi.h>

namespace gpcl::detail {
class win_process : noncopyable
{
  PROCESS_INFORMATION process_information_;
  DWORD exit_code_;

public:
  using native_handle_type = LPPROCESS_INFORMATION;

  struct command_line_tag
  {
  };

  win_process()
  {
    process_information_.hProcess = INVALID_HANDLE_VALUE;
    process_information_.hThread = INVALID_HANDLE_VALUE;
  }

  explicit win_process(command_line_tag, const char *cmdline)
  {
    STARTUPINFOA startup_info{};
    startup_info.cb = sizeof(startup_info);

    if (!CreateProcessA(nullptr, const_cast<char *>(cmdline), nullptr, nullptr,
                        FALSE, 0, nullptr, nullptr, &startup_info,
                        &process_information_))
    {
      throw_system_error("CreateProcessA");
    }
  }

  ~win_process()
  {
    if (joinable())
    {
      terminate();
      if (!try_join_for(chrono::seconds(2)))
      {
        kill();
        join();
      }
    }
  }

  /// <summary>
  /// Determines if the thread can be joined.
  /// </summary>
  /// <returns></returns>
  bool joinable() const
  {
    return process_information_.hProcess != INVALID_HANDLE_VALUE &&
           process_information_.hThread != INVALID_HANDLE_VALUE;
  }

  /// <summary>
  /// Notify the process to terminate.
  /// </summary>
  void terminate() {}

  void join() { try_join_for_impl(INFINITE); }

  bool try_join_for(chrono::milliseconds ms)
  {
    return try_join_for_impl(narrow_cast<DWORD>(ms.count()));
  }

  bool try_join_for_impl(DWORD ms)
  {
    GPCL_ASSERT(joinable());
    switch (WaitForSingleObject(process_information_.hProcess, ms))
    {
    case WAIT_OBJECT_0:
      if (!GetExitCodeProcess(process_information_.hProcess, &exit_code_))
      {
        throw_system_error("GetExitCodeProcess");
      }

      CloseHandle(process_information_.hProcess);
      CloseHandle(process_information_.hThread);
      process_information_.hProcess = INVALID_HANDLE_VALUE;
      process_information_.hThread = INVALID_HANDLE_VALUE;
      return TRUE;

    case WAIT_TIMEOUT:
      return FALSE;

    case WAIT_FAILED:
      throw_system_error("WaitForSingleObject");

    default:
      GPCL_UNREACHABLE("unexpected return value");
    }
  }

  void kill() { ::TerminateProcess(process_information_.hProcess, 255); }

  UINT exit_code() const
  {
    GPCL_ASSERT(!joinable());
    return exit_code_;
  }
};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_WIN_PROCESS_HPP
