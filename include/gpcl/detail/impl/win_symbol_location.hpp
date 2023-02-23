//
// win_symbol_location.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2023 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_SYMBOL_LOCATION_HPP
#define GPCL_DETAIL_IMPL_WIN_SYMBOL_LOCATION_HPP

#include <gpcl/detail/win_symbol_location.hpp>

#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/narrow_cast.hpp>
#include <gpcl/out_ptr.hpp>

namespace gpcl {
namespace detail {

template <typename T>
std::string win_symbol_location(const T &x, error_code &e)
{
  module_handle handle;
  if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                          reinterpret_cast<LPCSTR>(std::addressof(x)),
                          gpcl::out_ptr<HMODULE>(handle)))
  {
    DWORD err = GetLastError();
    e = error_code{gpcl::narrow_cast<int>(err), system_category()};
    return "";
  }

  std::string loc;
  loc.resize(PATH_MAX);
  DWORD size = ::GetModuleFileNameA(handle.get(), &loc[0], loc.size());
  while (size == loc.size())
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
      break;

    loc.resize(loc.size() * 2);
    size = ::GetModuleFileNameA(handle.get(), &loc[0], loc.size());
  }
  if (size == 0)
  {
    DWORD err = GetLastError();
    e = error_code{gpcl::narrow_cast<int>(err), system_category()};
    return "";
  }
  loc.resize(size);

  e = {};
  return loc;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_SYMBOL_LOCATION_HPP
