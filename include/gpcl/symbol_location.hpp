//
// symbol_location.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2023-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SYMBOL_LOCATION_HPP
#define GPCL_SYMBOL_LOCATION_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/error.hpp>
#ifdef GPCL_WINDOWS
#  include <gpcl/detail/win_symbol_location.hpp>
#elif defined(GPCL_POSIX)
#  include <gpcl/detail/posix_symbol_location.hpp>
#endif

namespace gpcl {

template <typename T>
std::string symbol_location(const T &x, error_code &e)
{
#ifdef GPCL_WINDOWS
  return gpcl::detail::win_symbol_location(x, e);
#elif defined(GPCL_POSIX)
  return gpcl::detail::posix_symbol_location(x, e);

#else
  e = make_error_code(errc::not_supported);
  return "";
#endif
}

template <typename T>
std::string symbol_location(const T &x)
{
  error_code ec;
  auto s = symbol_location(x, ec);
  if (ec)
    GPCL_THROW(system_error{ec, "symbol_location"});
  return s;
}

} // namespace gpcl

#endif // GPCL_SYMBOL_LOCATION_HPP
