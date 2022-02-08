//
// once_flag.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ONCE_FLAG_HPP
#define GPCL_ONCE_FLAG_HPP

#include <gpcl/detail/config.hpp>

#if defined(GPCL_POSIX)
#  define GPCL_ONCE_FLAG
#  include <gpcl/detail/posix_once_flag.hpp>
#elif defined(GPCL_WINDOWS)
#  define GPCL_ONCE_FLAG
#  include <gpcl/detail/win_once_flag.hpp>
#endif

namespace gpcl {

#if defined(GPCL_POSIX)
using once_flag = detail::posix_once_flag;
#elif defined(GPCL_WINDOWS)
using once_flag = detail::win_once_flag;
#endif

#ifdef GPCL_ONCE_FLAG
template <typename Callable, typename... Args>
void call_once(once_flag &flag, Callable &&callable, Args &&...args);
#endif

} // namespace gpcl

#endif // GPCL_ONCE_FLAG_HPP
