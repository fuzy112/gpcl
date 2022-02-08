//
// stacktrace.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_STACKTRACE_HPP
#define GPCL_STACKTRACE_HPP

#include <gpcl/basic_stacktrace.hpp>
#include <gpcl/default_allocator.hpp>

namespace gpcl {

#ifdef GPCL_STACKTRACE
using stacktrace = basic_stacktrace<default_allocator<stacktrace_entry>>;
#endif

} // namespace gpcl

#endif // GPCL_STACKTRACE_HPP
