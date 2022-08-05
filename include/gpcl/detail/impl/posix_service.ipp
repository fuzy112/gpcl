//
// posix_service.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_SERVICE_IPP
#define GPCL_DETAIL_IMPL_POSIX_SERVICE_IPP

#include <gpcl/detail/posix_service.hpp>

#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>

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
    GPCL_CATCH(...)
    {
      fd_max = 8192;
    }
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
    struct sigaction act = {};
    GPCL_THROW_LAST_ERROR_IF(sigemptyset(&act.sa_mask) < 0);
    act.sa_handler = SIG_DFL;
    GPCL_THROW_LAST_ERROR_IF(sigaction(sig, &act, nullptr) < 0 &&
                             errno != EINVAL);
  }
}

inline void reset_signal_mask()
{
  sigset_t mask;
  GPCL_THROW_LAST_ERROR_IF(::sigemptyset(&mask) < 0);
  GPCL_THROW_LAST_ERROR_IF(::sigprocmask(SIG_SETMASK, &mask, nullptr) < 0);
}

inline void sanitize_environ()
{
  const std::pair<const char *, const char *> envvars[] = {
      {"LD_PRELOAD", nullptr},
      {"PATH", "/usr/bin:/bin"},
      {"LD_LIBRARY_PATH", nullptr},
      {"LC_ALL", "C"},
      {"DISPLAY", nullptr},
      {"WAYLAND_DISPLAY", nullptr},
      {"XDG_RUNTIME_DIR", nullptr},
      {"XDG_CONFIG_DIR", nullptr},
      {"SHELL", nullptr},
      {"EDITOR", nullptr},
      {"HOME", nullptr},
      {"LANG", "en_US.UTF-8"},
      {"XDG_SESSION_CLASS", nullptr},
      {"XDG_SESSION_ID", nullptr},
      {"XDG_SESSION_TYPE", nullptr},
  };

  for (auto p : envvars)
  {
    if (p.second)
    {
      GPCL_THROW_LAST_ERROR_IF(::setenv(p.first, p.second, 1) < 0);
    }
    else
    {
      GPCL_THROW_LAST_ERROR_IF(::unsetenv(p.first) < 0);
    }
  }
}

inline int mkdir_with_parents(const char *path, mode_t mode)
{
  char *p;
  char *next;
  int ret = 0;

  char *buffer = strdup(path);
  int fd = -1;
  int dirfd = AT_FDCWD;

  if (!buffer)
    return -errno;

  next = buffer;
  do
  {
    p = next;
    next = strchr(p, '/');
    if (next)
      *next++ = '\0';

  retry:
    if (strlen(p) == 0)
      continue;
    fd = openat(dirfd, p, O_DIRECTORY | O_RDONLY | O_CLOEXEC);
    if (fd >= 0)
    {
      if (dirfd >= 0)
        close(dirfd);
      dirfd = fd;
      continue;
    }
    else if (errno == ENOENT)
    {
      if (mkdirat(dirfd, p, mode) < 0 && errno != EEXIST)
        goto err;
      goto retry;
    }
    else
    {
      goto err;
    }

  } while (next);
  errno = 0;

err:
  ret = -errno;
  if (fd >= 0)
    close(fd);
  if (dirfd >= 0)
    close(dirfd);
  free(buffer);
  errno = -ret;
  return ret;
}

void gpcl_posix_service_signal_handler(int sig)
{
  GPCL_ASSERT(posix_service::instance_);

  switch (sig)
  {
  case SIGINT:
  case SIGTERM:
    posix_service::instance_->do_stop();
    break;

  case SIGHUP:
    posix_service::instance_->do_restart();
    break;

  default:
    raise(sig);
    break;
  }
}

inline void install_service_signal_handler()
{
  struct sigaction act = {};
  act.sa_handler = &detail::gpcl_posix_service_signal_handler;
  GPCL_THROW_LAST_ERROR_IF(sigemptyset(&act.sa_mask) < 0);
  GPCL_THROW_LAST_ERROR_IF(sigaction(SIGINT, &act, nullptr) < 0);
  GPCL_THROW_LAST_ERROR_IF(sigaction(SIGTERM, &act, nullptr) < 0);
  GPCL_THROW_LAST_ERROR_IF(sigaction(SIGHUP, &act, nullptr) < 0);
}

void posix_service::start()
{
  uid_t euid = geteuid();

l_restart:
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

  GPCL_THROW_LAST_ERROR_IF(::seteuid(::getuid()) < 0);

  instance_ = this;

  install_service_signal_handler();

  do_start();

  GPCL_TRY
  {
    run();
  }
  GPCL_CATCH(...)
  {
    std::terminate();
  }
  GPCL_CATCH_END

  GPCL_THROW_LAST_ERROR_IF(seteuid(euid) < 0);

  if (need_restart_)
  {
    need_restart_ = false;
    goto l_restart;
  }

  exit(0);
}

void posix_service::parent()
{
  wrfd_.reset();

  int wstatus = 0;
  GPCL_THROW_LAST_ERROR_IF(waitpid(pid_.get(), &wstatus, 0) < 0);
  pid_.release();

  int error = {};
  ssize_t len = {};

  GPCL_THROW_LAST_ERROR_IF((len = ::read(rdfd_.get(), &error, sizeof(error))) <
                           0);
  if (len != sizeof(error))
    GPCL_THROW_ERRNO(EINVAL, "parent:read");
  if (error)
    GPCL_THROW_ERRNO(error, "start");
  exit(0);
}

void posix_service::child1()
{
  GPCL_TRY
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
  GPCL_CATCH(system_error & ex)
  {
    auto ec = ex.code();
    write_error(ec);
  }
  GPCL_AND_CATCH(std::bad_alloc &)
  {
    write_error(error_code(ENOMEM, system_category()));
  }
  GPCL_AND_CATCH(...)
  {
    write_error(make_error_code(errc::invalid_argument));
  }
  GPCL_CATCH_END
}

void posix_service::child2()
{
  ::umask(0);

  GPCL_THROW_LAST_ERROR_IF(chdir("/") < 0);

  auto out_file_path = out_file();
  GPCL_THROW_LAST_ERROR_IF(
      detail::mkdir_with_parents(::dirname(&out_file_path[0]), 0751) < 0);

  auto err_file_path = out_file();
  GPCL_THROW_LAST_ERROR_IF(
      detail::mkdir_with_parents(::dirname(&err_file_path[0]), 0751) < 0);

  GPCL_THROW_LAST_ERROR_IF(::close(STDIN_FILENO) < 0);
  GPCL_THROW_LAST_ERROR_IF(::close(STDOUT_FILENO) < 0);
  GPCL_THROW_LAST_ERROR_IF(::close(STDERR_FILENO) < 0);
  GPCL_VERIFY(::open("/dev/null", O_RDONLY | O_NOCTTY) == STDIN_FILENO);
  GPCL_VERIFY(::open(out_file().c_str(), O_CREAT | O_WRONLY, (mode_t)0640) ==
              STDOUT_FILENO);
  GPCL_VERIFY(::open(err_file().c_str(), O_CREAT | O_WRONLY, (mode_t)0640) ==
              STDERR_FILENO);

  pidfilefd_.emplace(pid_file());
}

void posix_service::write_error(error_code ec)
{
  GPCL_ASSERT(strcmp(ec.category().name(), "system") == 0 ||
              strcmp(ec.category().name(), "generic") == 0);
  int buf = ec.value();
  GPCL_THROW_LAST_ERROR_IF(::write(wrfd_.get(), &buf, sizeof(buf)) < 0);

  if (ec)
    _exit(1);

  wrfd_.reset();
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_SERVICE_IPP
