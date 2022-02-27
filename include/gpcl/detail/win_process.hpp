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
class win_process : noncopyable
{
  PROCESS_INFORMATION process_information_;
  DWORD exit_code_ = DWORD(-1);

public:
  using native_handle_type = LPPROCESS_INFORMATION;

  inline static constexpr STARTUPINFOA default_startup_info{
      sizeof(default_startup_info),
  };

  // clang-format off
  struct options
  {
    /* [in, optional]      */ LPCSTR                lpApplicationName{};
    /* [in, out, optional] */ LPSTR                 lpCommandLine{};
    /* [in, optional]      */ LPSECURITY_ATTRIBUTES lpProcessAttributes{};
    /* [in, optional]      */ LPSECURITY_ATTRIBUTES lpThreadAttributes{};
    /* [in]                */ BOOL                  bInheritHandles{};
    /* [in]                */ DWORD                 dwCreationFlags{};
    /* [in, optional]      */ LPVOID                lpEnvironment{};
    /* [in, optional]      */ LPCSTR                lpCurrentDirectory{};
    /* [in]                */ LPSTARTUPINFOA        lpStartupInfo{&default_startup_info};
  };
  // clang-format on

  constexpr win_process() noexcept
  {
    process_information_.hProcess = INVALID_HANDLE_VALUE;
    process_information_.hThread = INVALID_HANDLE_VALUE;
  }

  explicit win_process(const options &opt) : win_process() { start(opt); }

  explicit win_process(span<std::string_view const> args,
                       options opt = options{})
      : win_process()
  {
    start(args, opt);
  }

  void start(span<std::string_view const> args, options opt = options{})
  {
    std::ostringstream ss;
    for (auto arg : args)
      ss << std::quoted(arg) << ' ';
    auto str = ss.str();

    opt.lpCommandLine = str.c_str();
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

  bool killed() const noexcept { return false; }

  int signal() const noexcept { return 0; }

  bool exited() const noexcept { return !joinable(); }

  UINT exit_code() const
  {
    GPCL_ASSERT(!joinable());
    return exit_code_;
  }
};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_WIN_PROCESS_HPP
