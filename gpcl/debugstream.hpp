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
