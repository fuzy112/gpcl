//
// posix_lock_file.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_LOCK_FILE_HPP
#define GPCL_DETAIL_POSIX_LOCK_FILE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/posix_mutex.hpp>
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/noncopyable.hpp>

namespace gpcl {
namespace detail {
class posix_lock_file : noncopyable
{
public:
  explicit posix_lock_file(std::string filename)
      : filename_(std::move(filename))
  {
  }

  ~posix_lock_file()
  {
    if (owns_lock())
    {
      unlock();
    }
  }

  bool owns_lock() const { return owns_lock_; }

  GPCL_DECL void lock();

  GPCL_DECL bool try_lock();

  GPCL_DECL void unlock();

private:
  GPCL_DECL void write_pid();

  using mutex_type = posix_normal_mutex;

  mutex_type mtx_;

  std::string filename_;
  unique_fd fd_;
  bool owns_lock_{false};
};
} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_LOCK_FILE_HPP
