//
// json_iomanip.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_JSON_IOMANIP_HPP
#define GPCL_JSON_IOMANIP_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/json_error.hpp>

#include <iostream>

namespace gpcl {

/// This enum type can be used as an IO manipulator to control the output
/// style of JSON values.
enum json_print_style : long
{
  json_print_compressed = 0,
  json_print_pretty = 1,
};

namespace detail {
inline int json_style_xalloc()
{
  static const std::ios_base::Init init;
  static const int v = std::ios_base::xalloc();
  return v;
}
} // namespace detail

template <typename CharType, typename Traits>
inline auto &operator<<(std::basic_ostream<CharType, Traits> &os,
                        json_print_style style)
{
  os.iword(detail::json_style_xalloc()) = static_cast<long>(style);
  return os;
}

inline json_print_style get_json_print_style(std::ios_base &os)
{
  return static_cast<json_print_style>(os.iword(detail::json_style_xalloc()));
}

} // namespace gpcl

#endif // GPCL_JSON_IOMANIP_HPP
