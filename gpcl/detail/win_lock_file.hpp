//
// win_lock_file.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_LOCK_FILE_HPP
#define GPCL_DETAIL_WIN_LOCK_FILE_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/noncopyable.hpp>
#include <Windows.h>

namespace gpcl {
namespace detail {
class win_lock_file : noncopyable
{
public:
  explicit win_lock_file(std::string filename)
      : filename_(std::move(filename)),
        handle_(INVALID_HANDLE_VALUE)
  {
  }

  ~win_lock_file()
  {
    if (owns_lock())
    {
      unlock();
    }
  }

  bool owns_lock() const { return handle_ != INVALID_HANDLE_VALUE; }

  GPCL_DECL void lock();

  GPCL_DECL bool try_lock();

  GPCL_DECL void unlock();

private:
  std::string filename_;
  ::HANDLE handle_;
};
} // namespace detail
} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/win_lock_file.ipp>
#endif

#endif
