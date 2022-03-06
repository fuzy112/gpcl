//
// posix_pid_file.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_PID_FILE_HPP
#define GPCL_DETAIL_POSIX_PID_FILE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/optional.hpp>
#include <gpcl/time.hpp>

namespace gpcl {
namespace detail {

class posix_pid_file
{
public:
  GPCL_DECL explicit posix_pid_file(std::string path, bool update_pid = true);

  GPCL_DECL ~posix_pid_file() noexcept;

  void write_pid(pid_t pid = ::getpid())
  {
    unique_lock_guard guard(*this);
    do_write_pid(pid);
    owns_pid_ = true;
  }

  pid_t read_pid()
  {
    lock_shared_wait();
    return do_read_pid();
  }

private:
  GPCL_DECL void lock_unique();

  GPCL_DECL void lock_shared();

  GPCL_DECL void lock_shared_wait();

  GPCL_DECL void do_write_pid(pid_t pid);

  GPCL_DECL pid_t do_read_pid();

  struct unique_lock_guard
  {
    posix_pid_file &pidfile_;

    explicit unique_lock_guard(posix_pid_file &pidfile) : pidfile_(pidfile)
    {
      pidfile_.lock_unique();
    }

    ~unique_lock_guard() { pidfile_.lock_shared(); }
  };

  std::string path_;
  unique_fd f_;
  bool owns_pid_{false};
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_PID_FILE_HPP
