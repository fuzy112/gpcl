//
// posix_process.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_PROCESS_HPP
#define GPCL_DETAIL_POSIX_PROCESS_HPP

#include <gpcl/clock.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/dynarray.hpp>
#include <gpcl/span.hpp>

#include <string>
#include <string_view>

#include <signal.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace gpcl {
namespace detail {

inline int pidfd_open(pid_t pid, unsigned int flags)
{
  return syscall(__NR_pidfd_open, pid, flags);
}

class posix_process
{
public:
  using native_handle_type = int;

  constexpr explicit posix_process() noexcept = default;

  posix_process(const char *file, char *const argv[]) : posix_process()
  {
    start(file, argv);
  }

  explicit posix_process(span<const char *const> args) : posix_process()
  {
    start(args);
  }

  explicit posix_process(span<const std::string> args) : posix_process()
  {
    start(args);
  }

  explicit posix_process(span<const std::string_view> args) : posix_process()
  {
    start(args);
  }

  ~posix_process()
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

  void start(span<const char *const> args)
  {
    dynarray<char *> argv;
    for (auto &arg : args)
      argv.push_back(const_cast<char *>(arg));
    argv.push_back(nullptr);

    start(argv[0], argv.data());
  }

  void start(span<std::string const> args)
  {
    dynarray<char *> argv;
    for (auto &arg : args)
      argv.push_back(const_cast<char *>(arg.c_str()));
    argv.push_back(nullptr);

    start(argv[0], argv.data());
  }

  void start(span<const std::string_view> args)
  {
    dynarray<std::string> owned_args;
    for (auto arg : args)
      owned_args.emplace_back(arg);

    dynarray<char *> argv;
    for (auto &arg : owned_args)
      argv.push_back(&arg[0]);
    argv.push_back(nullptr);

    start(argv[0], argv.data());
  }

  void start(const char *file, char *const argv[])
  {
    GPCL_ASSERT(pid_ < 0);

    GPCL_THROW_LAST_ERROR_IF((pid_ = fork()) < 0);

    if (pid_ == 0)
      GPCL_TRY { GPCL_THROW_LAST_ERROR_IF(execvp(file, argv) < 0); }
    GPCL_CATCH(...) { raise(SIGABRT); }
    GPCL_CATCH_END

    GPCL_ASSERT(pid_ > 0);
  }

  bool joinable() const noexcept { return pid_ > 0; }

  void join()
  {
    GPCL_ASSERT(joinable());
    GPCL_THROW_LAST_ERROR_IF(waitpid(pid_, &wstatus_, 0) < 0);
    pid_ = terminated_process;
  }

  bool try_join()
  {
    GPCL_ASSERT(joinable());
    int result{};
    GPCL_THROW_LAST_ERROR_IF((result = waitpid(pid_, &wstatus_, WNOHANG)) < 0);
    if (result == pid_)
    {
      pid_ = terminated_process;
      return true;
    }
    GPCL_ASSERT(result == 0);
    return false;
  }

  bool try_join_for_impl(const timespec &ts)
  {
    process_waiter waiter(pid_);
    waiter.wait_for(ts);
    return try_join();
  }

  bool try_join_for(chrono::milliseconds timeout)
  {
    timespec ts;
    ts.tv_sec = timeout.count() / 1000;
    ts.tv_nsec = timeout.count() % 1000 * 1000000;
    return try_join_for_impl(ts);
  }

  void terminate()
  {
    GPCL_ASSERT(joinable());
    GPCL_THROW_LAST_ERROR_IF(::kill(pid_, SIGTERM) < 0);
  }

  void kill()
  {
    GPCL_ASSERT(joinable());
    GPCL_THROW_LAST_ERROR_IF(::kill(pid_, SIGKILL) < 0);
  }

  bool killed() const noexcept
  {
    GPCL_ASSERT(pid_ == terminated_process);
    return WIFSIGNALED(wstatus_);
  }

  bool exited() const noexcept
  {
    GPCL_ASSERT(pid_ == terminated_process);
    return WIFEXITED(wstatus_);
  }

  int signal() const noexcept
  {
    GPCL_ASSERT(killed());
    return WTERMSIG(wstatus_);
  }

  int exit_code() const noexcept
  {
    GPCL_ASSERT(exited());
    return WEXITSTATUS(wstatus_);
  }

private:
  static constexpr int not_a_process = -1;
  static constexpr int terminated_process = -2;

  int pid_{not_a_process};
  int wstatus_{};

  class process_waiter
  {
    int pidfd_{-1};

  public:
    explicit process_waiter(int pid)
    {
      GPCL_THROW_LAST_ERROR_IF((pidfd_ = pidfd_open(pid, 0)) < 0);
    }

    ~process_waiter() { close(pidfd_); }

    void wait_for(const timespec &ts)
    {
      fd_set set;
      FD_ZERO(&set);
      FD_SET(pidfd_, &set);
      int nfds = pidfd_ + 1;

      GPCL_THROW_LAST_ERROR_IF(
          pselect(nfds, &set, nullptr, nullptr, &ts, nullptr) < 0);
    }
  };
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_PROCESS_HPP
