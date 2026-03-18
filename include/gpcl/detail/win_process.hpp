//
// win_process.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
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

inline static STARTUPINFOA default_startup_info{
    sizeof(default_startup_info),
};

// clang-format off
struct win_process_options
{
  /* [in, optional]      */ LPCSTR                lpApplicationName{};
  /* [in, out, optional] */ LPSTR                 lpCommandLine{};
  /* [in, optional]      */ LPSECURITY_ATTRIBUTES lpProcessAttributes{};
  /* [in, optional]      */ LPSECURITY_ATTRIBUTES lpThreadAttributes{};
  /* [in]                */ BOOL                  bInheritHandles{};
  /* [in]                */ DWORD                 dwCreationFlags{};
  /* [in, optional]      */ LPVOID                lpEnvironment{};
  /* [in, optional]      */ LPCSTR                lpCurrentDirectory{};
  /* [in]                */ LPSTARTUPINFOA        lpStartupInfo{reinterpret_cast<LPSTARTUPINFOA>(&default_startup_info)};
};
// clang-format on

class win_process : noncopyable
{
  valid_handle process_;
  valid_handle thread_;

  DWORD pid_{};
  DWORD tid_{};

  DWORD exit_code_ = DWORD(-1);

public:
  using native_handle_type = LPPROCESS_INFORMATION;

  using options = win_process_options;

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

    PROCESS_INFORMATION info{};
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
                        &info                    // process information
                        ));
    process_.reset(info.hProcess);
    thread_.reset(info.hThread);

    pid_ = info.dwProcessId;
    tid_ = info.dwThreadId;
  }

  win_process(win_process &&other) noexcept
      : process_(std::move(other.process_)),
        thread_(std::move(other).thread_),
        pid_(other.pid_),
        tid_(other.tid_)
  {
    other.pid_ = 0;
    other.tid_ = 0;
  }

  win_process &operator=(win_process &&other) noexcept
  {
    swap(other);
    return *this;
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

  void swap(win_process &other) noexcept
  {
    using gpcl::swap;

    swap(process_, other.process_);
    swap(thread_, other.thread_);
    swap(pid_, other.pid_);
    swap(tid_, other.tid_);
  }

  friend void swap(win_process &x, win_process &y) noexcept { x.swap(y); }

  /// <summary>
  /// Determines if the thread can be joined.
  /// </summary>
  /// <returns></returns>
  bool joinable() const { return !!process_ && !!thread_; }

  /// <summary>
  /// Notify the process to terminate.
  /// </summary>
  void terminate()
  {
    GPCL_THROW_LAST_ERROR_IF(!EnumWindows(&enum_window_proc, (LPARAM)&pid_));
    PostThreadMessageA(tid_, WM_QUIT, 0, 0);
  }

  static BOOL CALLBACK enum_window_proc(HWND hwnd, LPARAM lParam)
  {
    DWORD pid{};
    auto tid = GetWindowThreadProcessId(hwnd, &pid);
    if (pid == *(DWORD const *)lParam)
    {
      GPCL_THROW_LAST_ERROR_IF(!PostMessageA(hwnd, WM_CLOSE, 1, 1));
      GPCL_THROW_LAST_ERROR_IF(!PostThreadMessageA(tid, WM_QUIT, 0, 0));
    }
    return TRUE;
  }

  void join() { GPCL_VERIFY(try_join_for_impl(INFINITE)); }

  [[nodiscard]] bool try_join() { return try_join_for_impl(0); }

  [[nodiscard]] bool try_join_for(chrono::milliseconds ms)
  {
    return try_join_for_impl(narrow_cast<DWORD>(ms.count()));
  }

  [[nodiscard]] bool try_join_for_impl(DWORD ms)
  {
    GPCL_ASSERT(joinable());
    DWORD wait_result;
    GPCL_THROW_LAST_ERROR_IF(
        (wait_result = WaitForSingleObject(process_.get(), ms)) == WAIT_FAILED);
    if (wait_result == WAIT_TIMEOUT)
      return false;
    GPCL_ASSERT(WAIT_OBJECT_0 == wait_result);

    GPCL_THROW_LAST_ERROR_IF(!GetExitCodeProcess(process_.get(), &exit_code_));

    process_.reset();
    thread_.reset();

    pid_ = 0;
    tid_ = 0;

    return TRUE;
  }

  void kill()
  {
    GPCL_THROW_LAST_ERROR_IF(!::TerminateProcess(process_.get(), 255));
  }

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
