//
// win_process.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_PROCESS_HPP
#define GPCL_DETAIL_WIN_PROCESS_HPP

#include <gpcl/clock.hpp>
#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/narrow_cast.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/span.hpp>

#include <iomanip>
#include <sstream>
#include <string_view>

#include <processthreadsapi.h>

namespace gpcl::detail {

GPCL_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wmissing-field-initializers")

class win_process : noncopyable
{
  PROCESS_INFORMATION process_information_{
      INVALID_HANDLE_VALUE,
      INVALID_HANDLE_VALUE,
  };
  DWORD exit_code_ = DWORD(-1);

public:
  using native_handle_type = LPPROCESS_INFORMATION;

  inline static STARTUPINFOA default_startup_info{
      sizeof(STARTUPINFOA),
  };

  // clang-format off
  struct options
  {
    /* [in, optional]      */ LPCSTR                lpApplicationName{nullptr};
    /* [in, out, optional] */ LPSTR                 lpCommandLine{};
    /* [in, optional]      */ LPSECURITY_ATTRIBUTES lpProcessAttributes{};
    /* [in, optional]      */ LPSECURITY_ATTRIBUTES lpThreadAttributes{};
    /* [in]                */ BOOL                  bInheritHandles{};
    /* [in]                */ DWORD                 dwCreationFlags{};
    /* [in, optional]      */ LPVOID                lpEnvironment{};
    /* [in, optional]      */ LPCSTR                lpCurrentDirectory{};
    /* [in]                */ LPSTARTUPINFOA        lpStartupInfo{const_cast<LPSTARTUPINFOA>(&default_startup_info)};
 
    options() = default;
  };
  // clang-format on

  win_process() noexcept = default;

  explicit win_process(const options &opt) { start(opt); }

  explicit win_process(span<std::string_view const> args,
                       const options &opt = options{})
  {
    start(args, opt);
  }

  explicit win_process(span<std::string const> args,
                       const options &opt = options{})
  {
    start(args, opt);
  }

  explicit win_process(span<const char *const> args, options opt = options{})
  {
    start(args, opt);
  }

  void start(span<const char *const> args, options opt = options{})
  {
    start(args.begin(), args.end(), opt);
  }

  void start(span<std::string const> args, options opt = options{})
  {
    start(args.begin(), args.end(), opt);
  }

  void start(span<std::string_view const> args, options opt = options{})
  {
    start(args.begin(), args.end(), opt);
  }

  template <typename It>
  void start(It first, It last, options opt = options{})
  {
    std::ostringstream ss;
    while (first != last)
      ss << std::quoted(*first++) << ' ';
    auto str = ss.str();

    opt.lpCommandLine = &str[0];
    start(opt);
  }

  void start(const options &opt)
  {
    GPCL_ASSERT(!joinable());
    GPCL_THROW_LAST_ERROR_IF(
        !CreateProcessA(opt.lpApplicationName,   // application name
                        opt.lpCommandLine,       // command line
                        opt.lpProcessAttributes, // process attributes
                        opt.lpThreadAttributes,  // thread attributes
                        opt.bInheritHandles,     // inherit handles
                        opt.dwCreationFlags,     // creation flags
                        opt.lpEnvironment,       // environment
                        opt.lpCurrentDirectory,  // current directory
                        opt.lpStartupInfo,       // startup info
                        &process_information_    // process information
                        ));
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
  void terminate()
  {
    GPCL_THROW_LAST_ERROR_IF(!EnumWindows(
        &enum_window_proc, (LPARAM)&process_information_.dwProcessId));
    PostThreadMessageA(process_information_.dwThreadId, WM_QUIT, 0, 0);
  }

  static BOOL CALLBACK enum_window_proc(HWND hwnd, LPARAM lParam)
  {
    DWORD pid{};
    auto tid = GetWindowThreadProcessId(hwnd, &pid);
    if (pid == *(DWORD const*)lParam)
    {
      GPCL_THROW_LAST_ERROR_IF(!PostMessageA(hwnd, WM_CLOSE, 1, 1));
      GPCL_THROW_LAST_ERROR_IF(!PostThreadMessageA(tid, WM_QUIT, 0, 0));
    }
    return TRUE;
  }

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

  bool killed() const noexcept { return false; }

  int signal() const noexcept { return 0; }

  bool exited() const noexcept { return !joinable(); }

  UINT exit_code() const
  {
    GPCL_ASSERT(!joinable());
    return exit_code_;
  }
};

GPCL_CLANG_SUPPRESS_WARNING_POP

} // namespace gpcl::detail

#endif // GPCL_DETAIL_WIN_PROCESS_HPP
