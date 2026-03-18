//
// win_module_handle.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2023-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_MODULE_HANDLE_HPP
#define GPCL_DETAIL_WIN_MODULE_HANDLE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>

#include <windows.h>

namespace gpcl {
namespace detail {
struct module_handle_traits
{
  using native_handle_type = HMODULE;

  static constexpr native_handle_type invalid_value{nullptr};

  static constexpr bool is_valid(native_handle_type h) { return !!h; }

  static void close(native_handle_type h) noexcept
  {
    GPCL_VERIFY(::FreeLibrary(h));
  }
};

typedef unique_handle<module_handle_traits> module_handle;

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_WIN_MODULE_HANDLE_HPP
