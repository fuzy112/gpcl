//
// pid_file.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PID_FILE_HPP
#define GPCL_PID_FILE_HPP

#include <gpcl/detail/config.hpp>

#if defined(GPCL_POSIX)
#  include <gpcl/detail/posix_pid_file.hpp>
#endif

namespace gpcl {

#if defined(GPCL_POSIX)
typedef detail::posix_pid_file pid_file;
#endif

} // namespace gpcl

#endif // GPCL_PID_FILE_HPP
