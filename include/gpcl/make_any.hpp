//
// make_any.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MAKE_ANY_HPP
#define GPCL_MAKE_ANY_HPP

#include <gpcl/any.hpp>
#include <gpcl/make_basic_any.hpp>

#include <initializer_list>
#include <utility>

namespace gpcl {

/// @relates gpcl::any
/// @relates gpcl::basic_any
template <typename T, typename... Args>
any make_any(Args &&... args)
{
  return make_basic_any<any, T>(std::forward<Args>(args)...);
}

/// @relates gpcl::any
/// @relates gpcl::basic_any
template <typename T, typename U, typename... Args>
any make_any(std::initializer_list<U> il, Args &&... args)
{
  return make_basic_any<any, T>(il, std::forward<Args>(args)...);
}

} // namespace gpcl

#endif // GPCL_MAKE_ANY_HPP
