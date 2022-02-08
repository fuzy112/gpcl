//
// unreachable.ipp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_UNREACHABLE_IPP
#define GPCL_DETAIL_IMPL_UNREACHABLE_IPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unreachable.hpp>
#include <iostream>

namespace gpcl {
namespace detail {

void unreachable_internal(czstring<> msg, czstring<> file,
                          unsigned line) noexcept
{
  if (msg)
    std::clog << msg << "\n";
  std::clog << "UNREACHABLE executed";
  if (file)
    std::clog << " at " << file << ":" << line;
  std::clog << "!\n" << std::flush;
  abort();
}

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_UNREACHABLE_IPP
