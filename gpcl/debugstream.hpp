//
// debugstream.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DEBUG_STREAM_HPP
#define GPCL_DEBUG_STREAM_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/osyncstream.hpp>

#ifdef GPCL_WINDOWS
#  include <gpcl/detail/win_debugstream.hpp>

namespace gpcl {
inline auto cdebug()
{
  return osyncstream(detail::cwindebug);
}
} // namespace gpcl

#else

#  include <iostream>

namespace gpcl {
inline auto cdebug()
{
  return osyncstream(std::clog);
}
} // namespace gpcl
#endif

#endif // GPCL_DEBUG_STREAM_HPP
