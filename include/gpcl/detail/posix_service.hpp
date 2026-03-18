//
// posix_service.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_SERVICE_HPP
#define GPCL_DETAIL_POSIX_SERVICE_HPP

#include <gpcl/atomic.hpp>
#include <gpcl/content_iterator.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/unique_dir.hpp>
#include <gpcl/pid_file.hpp>

#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace gpcl {
namespace detail {

struct pid_traits
{
  using native_handle_type = pid_t;

  static inline constexpr native_handle_type invalid_value{-1};

  static constexpr bool is_valid(pid_t p) noexcept { return p >= 0; }

  static void close(pid_t p)
  {
    if (p == 0)
      return;
    int wstatus = 0;
    int r;
    GPCL_THROW_LAST_ERROR_IF((r = ::waitpid(p, &wstatus, WNOHANG)) < 0);
    if (r)
      return;
    GPCL_THROW_LAST_ERROR_IF(::kill(p, SIGTERM) < 0);
    GPCL_THROW_LAST_ERROR_IF(::waitpid(p, &wstatus, 0) < 0);
  }
};

using unique_pid = unique_handle<pid_traits>;

extern "C" GPCL_DECL void gpcl_posix_service_signal_handler(int sig);

class posix_service
{
  friend void detail::gpcl_posix_service_signal_handler(int sig);

  std::string service_name_;

  unique_pid pid_{};
  unique_fd rdfd_;
  unique_fd wrfd_;

  optional<gpcl::pid_file> pidfilefd_{};

  static inline posix_service *instance_{nullptr};
  atomic_char run_{false};
  atomic_char need_restart_{false};

public:
  explicit posix_service(std::string name) : service_name_(std::move(name)) {}

  virtual ~posix_service() = default;

  GPCL_DECL void start();

  void stop()
  {
    gpcl::pid_file pidfile(pid_file(), false);
    int pid = pidfile.read_pid();
    GPCL_THROW_LAST_ERROR_IF(::kill(pid, SIGTERM) < 0);
  }

  void restart()
  {
    GPCL_TRY
    {
      gpcl::pid_file pidfile(pid_file(), false);
      int pid = pidfile.read_pid();
      GPCL_THROW_LAST_ERROR_IF(::kill(pid, SIGHUP) < 0);
    }
    GPCL_CATCH(...)
    {
      start();
    }
    GPCL_CATCH_END
  }

  bool is_stopped() const { return do_is_stopped(); }

  std::string name() const { return service_name_; }

  std::string pid_file() const
  {
#if defined(__linux__)
    return "/run/" + name() + ".pid";
#else
    return "/var/run/" + name() + ".pid";
#endif
  }

  std::string err_file() const
  {
    return "/var/log/" + name() + "/err.log";
  }

  std::string out_file() const
  {
    return "/var/log/" + name() + "/out.log";
  }

protected:
  virtual void do_start()
  {
    run_.store(true);
    notify_success();
  }
  virtual void run() noexcept {}
  virtual void do_stop() noexcept
  {
    run_.store(false);
  }
  virtual void do_restart() noexcept
  {
    need_restart_.store(true);
    do_stop();
  }

  virtual bool do_is_stopped() const noexcept
  {
    return !run_;
  }

  void notify_success()
  {
    write_error(error_code());
  }

private:
  GPCL_NORETURN GPCL_DECL void parent();

  GPCL_DECL void child1();

  GPCL_DECL void child2();

  GPCL_DECL void write_error(error_code ec);
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_SERVICE_HPP
