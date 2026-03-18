//
// apply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_APPLY_HPP
#define GPCL_APPLY_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/invoke.hpp>

#include <tuple>

namespace gpcl {

namespace detail {

template <typename F, typename Tuple, std::size_t... Is>
constexpr auto apply_impl(F &&f, Tuple &&t, std::index_sequence<Is...>)
    -> decltype(gpcl::invoke(std::forward<F>(f),
                             std::get<Is>(std::forward<Tuple>(t))...))
{
  return gpcl::invoke(std::forward<F>(f),
                      std::get<Is>(std::forward<Tuple>(t))...);
}

} // namespace detail

template <typename F, typename Tuple>
constexpr auto apply(F &&f, Tuple &&t) -> decltype(detail::apply_impl(
    std::forward<F>(f), std::forward<Tuple>(t),
    std::make_integer_sequence<std::size_t, std::tuple_size<Tuple>::value>()))
{
  return detail::apply_impl(
      std::forward<F>(f), std::forward<Tuple>(t),
      std::make_integer_sequence<std::size_t,
                                 std::tuple_size<typename std::remove_reference<
                                     Tuple>::type>::value>());
}

} // namespace gpcl

#endif // GPCL_APPLY_HPP
