//
// access_mode.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ACCESS_MODE_HPP
#define GPCL_ACCESS_MODE_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {
enum class access_mode
{
  read_only,
  read_write,
};

constexpr access_mode read_only = access_mode::read_only;
constexpr access_mode read_write = access_mode::read_write;

} // namespace gpcl

#endif // GPCL_ACCESS_MODE_HPP
