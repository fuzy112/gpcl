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
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/dynarray.hpp>
#include <gpcl/span.hpp>

#include <string>
#include <string_view>

#include <signal.h>
#include <sys/select.h>
#include <sys/signalfd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __NR_pidfd_open
#  define GPCL_DETAIL_USE_PIDFD
#endif

namespace gpcl {
namespace detail {

#ifdef GPCL_DETAIL_USE_PIDFD
inline int pidfd_open(pid_t pid, unsigned int flags)
{
  return syscall(__NR_pidfd_open, pid, flags);
}
#endif

struct posix_process_options
{
  const char *file{};
  char *const *argv{};

  span<std::pair<int, int>> fds{};

  const char *working_directory{};

  bool close_files{false};
};

class posix_process
{
public:
  using native_handle_type = int;

  using options = posix_process_options;

  constexpr explicit posix_process() noexcept = default;

  posix_process(const char *file, char *const argv[],
                const options &opt = options{})
      : posix_process()
  {
    start(file, argv, opt);
  }

  explicit posix_process(span<const char *const> args,
                         const options &opt = options{})
      : posix_process()
  {
    start(args, opt);
  }

  explicit posix_process(span<const std::string> args,
                         const options &opt = options{})
      : posix_process()
  {
    start(args, opt);
  }

  explicit posix_process(span<const std::string_view> args,
                         const options &opt = options{})
      : posix_process()
  {
    start(args, opt);
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

  void start(span<const char *const> args, const options &opt = options{})
  {
    dynarray<char *> argv;
    for (auto &arg : args)
      argv.push_back(const_cast<char *>(arg));
    argv.push_back(nullptr);

    start(argv[0], argv.data(), opt);
  }

  void start(span<std::string const> args, const options &opt = options{})
  {
    dynarray<char *> argv;
    for (auto &arg : args)
      argv.push_back(const_cast<char *>(arg.c_str()));
    argv.push_back(nullptr);

    start(argv[0], argv.data(), opt);
  }

  void start(span<const std::string_view> args, const options &opt = options{})
  {
    dynarray<std::string> owned_args;
    for (auto arg : args)
      owned_args.emplace_back(arg);

    dynarray<char *> argv;
    for (auto &arg : owned_args)
      argv.push_back(&arg[0]);
    argv.push_back(nullptr);

    start(argv[0], argv.data(), opt);
  }

  void start(const char *file, char *const argv[], options opt = options{})
  {
    opt.file = file;
    opt.argv = argv;
    start(opt);
  }

  void start(const options &opt)
  {
    GPCL_ASSERT(pid_ < 0);

    GPCL_THROW_LAST_ERROR_IF((pid_ = fork()) < 0);

    if (pid_ == 0)
      GPCL_TRY
      {
        for (auto &[oldfd, newfd] : opt.fds)
        {
          // here fd will leak if error occurs, but it doesn't
          // matter because the process is about to exit.
          GPCL_THROW_LAST_ERROR_IF(dup2(oldfd, newfd) < 0);
        }

        if (opt.working_directory)
        {
          GPCL_THROW_LAST_ERROR_IF(chdir(opt.working_directory));
        }

        if (opt.close_files)
        {
          int max_fd = sysconf(_SC_OPEN_MAX);
          if (max_fd == -1)
            max_fd = 8192;

          for (int fd = 0; fd < max_fd; ++fd)
          {
            for (auto &[_, newfd] : opt.fds)
              if (fd == newfd)
                goto next;

            GPCL_THROW_LAST_ERROR_IF(::close(fd) < 0 && errno != EBADF);
          next:;
          }
        }

        GPCL_THROW_LAST_ERROR_IF(execvp(opt.file, opt.argv) < 0);
      }
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

  [[nodiscard]] bool try_join()
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

  [[nodiscard]] bool try_join_for_impl(const timespec &ts)
  {
    process_waiter waiter(pid_);

    waiter.wait_for(ts);
    return try_join();
  }

  [[nodiscard]] bool try_join_for(chrono::milliseconds timeout)
  {
    timespec ts;
    ts.tv_sec = timeout.count() / 1000;
    ts.tv_nsec = timeout.count() % 1000 * 1000000;
    return try_join_for_impl(ts);
  }

  void terminate() { send_signal(SIGTERM); }

  void kill() { send_signal(SIGKILL); }

  void send_signal(int sig)
  {
    GPCL_ASSERT(joinable());
    GPCL_THROW_LAST_ERROR_IF(::kill(pid_, sig) < 0);
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

  int native_handle() const noexcept { return pid_; }

private:
  static constexpr int not_a_process = -1;
  static constexpr int terminated_process = -2;

  int pid_{not_a_process};
  int wstatus_{};

#ifdef GPCL_DETAIL_USE_PIDFD
  class process_waiter
  {
    unique_fd pidfd_;

  public:
    explicit process_waiter(int pid) : pidfd_(pidfd_open(pid, 0))
    {
      GPCL_THROW_LAST_ERROR_IF(!pidfd_);
    }

    void wait_for(const timespec &ts)
    {
      fd_set set;
      FD_ZERO(&set);
      FD_SET(pidfd_.get(), &set);
      int nfds = pidfd_.get() + 1;

      sigset_t mask;
      GPCL_THROW_LAST_ERROR_IF(sigemptyset(&mask) < 0);
      GPCL_THROW_LAST_ERROR_IF(sigaddset(&mask, SIGCHLD) < 0);
      GPCL_THROW_LAST_ERROR_IF(
          pselect(nfds, &set, nullptr, nullptr, &ts, &mask) < 0);
    }
  };
#else
  class process_waiter
  {
    int pid_{-1};
    sigset_t mask;
    sigset_t old_mask;

  public:
    explicit process_waiter(int pid) : pid_{pid}
    {
      GPCL_THROW_LAST_ERROR_IF(sigemptyset(&mask) < 0);
      GPCL_THROW_LAST_ERROR_IF(sigaddset(&mask, SIGCHLD) < 0);
      GPCL_THROW_LAST_ERROR_IF(sigprocmask(SIG_BLOCK, &mask, &old_mask) < 0);
    }

    ~process_waiter() { sigprocmask(SIG_SETMASK, &old_mask, nullptr); }

    void wait_for(const timespec &ts)
    {
      /// calculate deadline
      timespec deadline;
      GPCL_THROW_LAST_ERROR_IF(clock_gettime(CLOCK_MONOTONIC, &deadline) < 0);
      deadline.tv_nsec += ts.tv_nsec;
      deadline.tv_sec += ts.tv_sec;
      deadline.tv_sec += deadline.tv_nsec / 1'000'000'000;
      deadline.tv_nsec %= 1'000'000'000;

      timespec remain = ts;
      const timespec zero = {};

      do
      {
        GPCL_THROW_LAST_ERROR_IF(sigtimedwait(&mask, NULL, &remain) < 0 &&
                                 errno != EAGAIN);

        siginfo_t info{};
        GPCL_THROW_LAST_ERROR_IF(
            waitid(P_PID, pid_, &info, WEXITED | WNOWAIT | WNOHANG) < 0);
        if (pid_ == info.si_pid)
          break;

        timespec now;
        GPCL_THROW_LAST_ERROR_IF(clock_gettime(CLOCK_MONOTONIC, &now) < 0);

        remain = deadline;
        timespec_sub(&remain, &now);
      } while (timespec_gt(&remain, &zero));
    }
  };

#endif
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_PROCESS_HPP
