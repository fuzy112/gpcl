//
// utility.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_UTILITY_HPP
#define GPCL_DETAIL_UTILITY_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/narrow_cast.hpp>

namespace gpcl {
namespace detail {

template <typename T, typename U>
inline constexpr T &&forward(U &&expr) noexcept
{
  return static_cast<T &&>(expr);
}

template <typename T>
inline constexpr T &&move(T &obj) noexcept
{
  return static_cast<T &&>(obj);
}

template <typename T, typename U>
inline constexpr T exchange(T &obj, U &&new_value)
{
  T old_value = detail::move(obj);
  obj = detail::forward<U>(new_value);
  return old_value;
}

using std::swap;

} // namespace detail
} // namespace gpcl

#endif
