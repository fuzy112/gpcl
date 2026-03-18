//
// process.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PROCESS_HPP
#define GPCL_PROCESS_HPP

#include <gpcl/detail/config.hpp>

#ifdef GPCL_POSIX
#  include <gpcl/detail/posix_process.hpp>
namespace gpcl {
using process = detail::posix_process;
}
#elif defined(GPCL_WINDOWS)
#  include <gpcl/detail/win_process.hpp>
namespace gpcl {
using process = detail::win_process;
}
#endif

#endif // GPCL_PROCESS_HPP
