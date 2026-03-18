//
// posix_symbol_location.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2023-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_SYMBOL_LOCATION_HPP
#define GPCL_DETAIL_IMPL_POSIX_SYMBOL_LOCATION_HPP

#include <gpcl/detail/posix_symbol_location.hpp>

#include <dlfcn.h>

namespace gpcl {
namespace detail {

template <typename T>
std::string posix_symbol_location(const T &x, error_code &e)
{
  Dl_info info;
  if (::dladdr(const_cast<T *>(&x), &info) == 0)
  {
    e = make_error_code(errc::bad_address);
    return "";
  }

  return info.dli_fname;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_SYMBOL_LOCATION_HPP
