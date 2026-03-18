//
// utility.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_UTILITY_HPP
#define GPCL_DETAIL_UTILITY_HPP

#include <gpcl/detail/config.hpp>

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
inline constexpr T exchange(T &obj, U &&new_value) noexcept(noexcept(obj = detail::forward<U>(new_value)))
{
  T old_value = detail::move(obj);
  obj = detail::forward<U>(new_value);
  return old_value;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_UTILITY_HPP
