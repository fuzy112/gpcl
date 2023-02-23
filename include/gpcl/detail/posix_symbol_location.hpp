//
// posix_symbol_location.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2023 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_SYMBOL_LOCATION_HPP
#define GPCL_DETAIL_POSIX_SYMBOL_LOCATION_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>

namespace gpcl {
namespace detail {

template <typename T>
std::string posix_symbol_location(const T &x, error_code &e);

}
} // namespace gpcl

#include <gpcl/detail/impl/posix_symbol_location.hpp>

#endif // GPCL_DETAIL_POSIX_SYMBOL_LOCATION_HPP
