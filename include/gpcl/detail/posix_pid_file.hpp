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
#include <gpcl/file.hpp>
#include <gpcl/optional.hpp>
#include <gpcl/time.hpp>

namespace gpcl {
namespace detail {

class posix_pid_file : gpcl::noncopyable
{
public:
  GPCL_DECL explicit posix_pid_file(std::string path);

  GPCL_DECL ~posix_pid_file() noexcept;

private:
  std::string path_;
  file f_;
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_POSIX_PID_FILE_HPP
