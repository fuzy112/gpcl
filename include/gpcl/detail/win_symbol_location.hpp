//
// win_symbol_location.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2023 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_SYMBOL_LOCATION_HPP
#define GPCL_DETAIL_WIN_SYMBOL_LOCATION_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/win_module_handle.hpp>
#include <string>

namespace gpcl {
namespace detail {

template <typename T>
std::string win_symbol_location(const T &x);

} // namespace detail
} // namespace gpcl

#include <gpcl/detail/impl/win_symbol_location.hpp>

#endif // GPCL_DETAIL_WIN_SYMBOL_LOCATION_HPP
