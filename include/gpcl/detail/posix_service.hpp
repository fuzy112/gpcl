#ifndef GPCL_DETAIL_POSIX_SERVICE_HPP
#define GPCL_DETAIL_POSIX_SERVICE_HPP

#include <gpcl/content_iterator.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/unique_dir.hpp>
#include <gpcl/pid_file.hpp>

#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace gpcl {
namespace detail {

class directory_entry_ref
{
  DIR *dir_{};
  ::dirent *ent_{};

public:
  directory_entry_ref() noexcept = default;

  explicit directory_entry_ref(DIR *dir) : dir_{dir} { next(); }

  void next()
  {
    errno = 0;
    ent_ = ::readdir(dir_);
    GPCL_THROW_LAST_ERROR_IF(ent_ == nullptr && errno != 0);
  }

  friend bool operator==(const directory_entry_ref &x,
                         const directory_entry_ref &y)
  {
    return (!x.ent_ && !y.ent_);
  }

  ino_t ino() const noexcept { return ent_->d_ino; }

  off_t off() const noexcept { return ent_->d_off; }

  unsigned short reclen() const noexcept { return ent_->d_reclen; }

  unsigned char type() const noexcept { return ent_->d_type; }

  const char *name() const noexcept { return ent_->d_name; }
};

inline namespace posix_service_detail {
inline void close_all_fd(int start = 3)
{
  const char *fd_dir_path = "/proc/self/fd";
#ifdef __FreeBSD__
  fd_dir_path = "/dev/fd";
#endif
  unique_dir fd_dir(opendir(fd_dir_path));

  if (fd_dir)
  {
    const int dirfd = ::dirfd(fd_dir.get());
    content_iterator<directory_entry_ref> iter{
        directory_entry_ref(fd_dir.get())};
    content_iterator<directory_entry_ref> end{directory_entry_ref()};
    while (iter != end)
    {
      int fd = atoi(iter->name());
      if (fd >= start && fd != dirfd)
        GPCL_THROW_LAST_ERROR_IF(::close(fd) < 0);
      ++iter;
    }
  }
  else
  {
    int fd_max;
    GPCL_TRY
    {
      struct ::rlimit lim = {};
      GPCL_THROW_LAST_ERROR_IF(getrlimit(RLIMIT_NOFILE, &lim) < 0);
      fd_max = lim.rlim_cur;
    }
    GPCL_CATCH(...) { fd_max = 8192; }
    GPCL_CATCH_END
    for (int fd = start; fd < fd_max; ++fd)
      GPCL_THROW_LAST_ERROR_IF(::close(fd) < 0 && errno != EINVAL &&
                               errno != EBADF);
  }
}

inline void reset_signal_handlers()
{
#ifdef _NSIG
  int nsig = _NSIG;
#elif defined(NSIG)
  int nsig = NSIG;
#else
  int nsig = 32;
#endif
  for (int sig = 1; sig < nsig; ++sig)
  {
    GPCL_THROW_LAST_ERROR_IF(SIG_ERR == ::signal(sig, SIG_DFL) &&
                             errno != EINVAL);
  }
}

inline void reset_signal_mask()
{
  sigset_t mask;
  GPCL_THROW_LAST_ERROR_IF(::sigemptyset(&mask) < 0);
  GPCL_THROW_LAST_ERROR_IF(::sigprocmask(SIG_SETMASK, &mask, nullptr) < 0);
}

inline void sanitize_environ() {}

inline void open_null(int fd, int flags)
{
  unique_fd tmpfd(open("/dev/null", O_NOCTTY | flags) < 0);
  GPCL_THROW_LAST_ERROR_IF(!tmpfd);
  GPCL_THROW_LAST_ERROR_IF(dup2(tmpfd.get(), fd) < 0);
  if (tmpfd.get() == fd)
    tmpfd.release();
}

struct error_data
{
  int code;
  const error_category *category;
};

extern "C" void service_signal_handler(int sig);

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

class posix_service
{
  const char *pidfile_{};
  unique_pid pid_{};
  unique_fd rdfd_;
  unique_fd wrfd_;

  unique_ptr<pid_file> pidfilefd_{};

  static inline posix_service *instance_{nullptr};
  std::atomic_bool run_{false};
  std::atomic_bool need_restart_{false};

public:
  posix_service(const char *pidfile = nullptr) : pidfile_(pidfile) {}

  friend void service_signal_handler(int sig)
  {
    GPCL_ASSERT(instance_);

    switch (sig)
    {
    case SIGINT:
    case SIGTERM:
      instance_->do_stop();
      break;

    case SIGHUP:
      instance_->do_restart();
      break;

    default:
      raise(sig);
      break;
    }
  }

  void start()
  {
    uid_t euid = geteuid();

  l_restart:
    GPCL_THROW_LAST_ERROR_IF(seteuid(euid) < 0);
    pidfilefd_.reset();
    rdfd_.reset();
    wrfd_.reset();
    close_all_fd();
    reset_signal_handlers();
    reset_signal_mask();
    sanitize_environ();

    int pipefd[2];
    GPCL_THROW_LAST_ERROR_IF(pipe2(pipefd, O_CLOEXEC) < 0);
    GPCL_ASSERT(pipefd[0] >= 3);
    GPCL_ASSERT(pipefd[1] >= 3);

    rdfd_.reset(pipefd[0]);
    wrfd_.reset(pipefd[1]);

    pid_.reset(::fork());
    GPCL_THROW_LAST_ERROR_IF(!pid_);
    if (pid_.get() > 0)
      return parent();

    child1();

    instance_ = this;

    ::signal(SIGTERM, &detail::service_signal_handler);
    ::signal(SIGINT, &detail::service_signal_handler);
    ::signal(SIGHUP, &detail::service_signal_handler);

    do_start();
    notify_success();
    run();

    if (need_restart_)
    {
      need_restart_ = false;
      goto l_restart;
    }
  }

  void stop()
  {
    pid_file pidfile(pidfile_, false);
    int pid = pidfile.read_pid();
    GPCL_THROW_LAST_ERROR_IF(::kill(pid, SIGTERM) < 0);
  }

  void restart()
  {
    GPCL_TRY
    {
      pid_file pidfile(pidfile_, false);
      int pid = pidfile.read_pid();
      GPCL_THROW_LAST_ERROR_IF(::kill(pid, SIGHUP) < 0);
    }
    GPCL_CATCH(...) { start(); }
    GPCL_CATCH_END
  }

  bool is_stopped() const { return do_is_stopped(); }

protected:
  virtual void do_start() { run_ = true; }
  virtual void run() {}
  virtual void do_stop() { run_ = false; }
  virtual void do_restart()
  {
    need_restart_ = true;
    do_stop();
  }

  virtual bool do_is_stopped() const { return !run_; }

  void notify_success() { write_error(error_code()); }

private:
  GPCL_NORETURN void parent()
  {
    wrfd_.reset();

    int wstatus = 0;
    GPCL_THROW_LAST_ERROR_IF(waitpid(pid_.get(), &wstatus, 0) < 0);
    pid_.release();

    error_data buf = {};
    ssize_t len = {};

    GPCL_THROW_LAST_ERROR_IF((len = ::read(rdfd_.get(), &buf, sizeof(buf))) <
                             0);
    if (len == 0)
      GPCL_THROW_ERRNO(EINVAL, __func__);
    GPCL_ASSERT(len == sizeof(buf));
    error_code ec(buf.code, system_category());
    if (ec)
      GPCL_THROW_SYSTEM_ERROR(buf.code, system_category(), __func__);
    exit(0);
  }

  void child1() GPCL_TRY
  {
    rdfd_.reset();

    int pipefd2[2];
    GPCL_THROW_LAST_ERROR_IF(::pipe2(pipefd2, O_CLOEXEC) < 0);
    GPCL_ASSERT(pipefd2[0] >= 3);
    GPCL_ASSERT(pipefd2[1] >= 3);

    unique_fd rdfd2(pipefd2[0]);
    unique_fd wrfd2(pipefd2[1]);

    GPCL_THROW_LAST_ERROR_IF(::setsid() < 0);

    pid_.reset(::fork());
    GPCL_THROW_LAST_ERROR_IF(!pid_);
    if (pid_.get() > 0)
      return exit(0);

    // in child2, wait child1 to exit
    wrfd2.reset();
    char dummy[1];
    GPCL_THROW_LAST_ERROR_IF(::read(rdfd2.get(), dummy, sizeof(dummy)) < 0);

    child2();
  }
  GPCL_CATCH(system_error &ex)
  {
    auto ec = ex.code();
    write_error(ec);
  }
  GPCL_CATCH(...) { write_error(error_code(ENOMEM, system_category())); }
  GPCL_CATCH_END

  void child2()
  {
    GPCL_THROW_LAST_ERROR_IF(::close(STDIN_FILENO) < 0);
    // GPCL_THROW_LAST_ERROR_IF(::close(STDOUT_FILENO) < 0);
    // GPCL_THROW_LAST_ERROR_IF(::close(STDERR_FILENO) < 0);
    GPCL_VERIFY(::open("/dev/null", O_RDONLY | O_NOCTTY) == STDIN_FILENO);
    // GPCL_VERIFY(::open("/dev/null", O_WRONLY | O_NOCTTY) == STDOUT_FILENO);
    // GPCL_VERIFY(::open("/dev/null", O_WRONLY | O_NOCTTY) == STDERR_FILENO);

    ::umask(0);

    GPCL_THROW_LAST_ERROR_IF(chdir("/") < 0);

    if (pidfile_)
      pidfilefd_ = gpcl::make_unique<pid_file>(pidfile_);

    GPCL_THROW_LAST_ERROR_IF(::seteuid(::getuid()) < 0);
  }

  void write_error(error_code ec)
  {
    GPCL_ASSERT(strcmp(ec.category().name(), "system") == 0 ||
                strcmp(ec.category().name(), "generic") == 0);
    error_data buf = {ec.value(), &ec.category()};
    GPCL_THROW_LAST_ERROR_IF(::write(wrfd_.get(), &buf, sizeof(buf)) < 0);

    if (ec)
      _exit(1);

    wrfd_.reset();
  }
};

} // namespace posix_service_detail

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_SERVICE_HPP
