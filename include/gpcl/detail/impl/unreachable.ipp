//
// unreachable.ipp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_UNREACHABLE_IPP
#define GPCL_DETAIL_IMPL_UNREACHABLE_IPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unreachable.hpp>
#include <gpcl/debugstream.hpp>

#include <cstdlib>

namespace gpcl {
namespace detail {

void unreachable_internal(gpcl::czstring<> message, source_location location) noexcept
{
  auto &&stream = cdebug();
  stream << message << "\n";
  stream << "UNREACHABLE executed";
  stream << " at " << location.file() << ":" << location.line() << " " << location.function();
  stream << "!\n" << std::flush;
  std::abort();
}

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_UNREACHABLE_IPP
