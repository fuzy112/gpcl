//
// basic_stacktrace.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BASIC_STACKTRACE_HPP
#define GPCL_BASIC_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>

#if defined(GPCL_WINDOWS)
#  include <gpcl/detail/win_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#elif defined(GPCL_POSIX)
#  include <gpcl/detail/posix_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#endif

#include <memory>

namespace gpcl {
#if defined(GPCL_WINDOWS)
template <typename Allocator>
using basic_stacktrace = detail::basic_win_stacktrace<Allocator>;
using stacktrace_entry = detail::win_stacktrace_entry;
#elif defined(GPCL_POSIX)
template <typename Allocator>
using basic_stacktrace = detail::basic_posix_stacktrace<Allocator>;
using stacktrace_entry = detail::posix_stacktrace_entry;
#endif

} // namespace gpcl

#endif // GPCL_BASIC_STACKTRACE_HPP
