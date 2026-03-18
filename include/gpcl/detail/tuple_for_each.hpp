//
// tuple_for_each.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_TUPLE_FOR_EACH_HPP
#define GPCL_DETAIL_TUPLE_FOR_EACH_HPP

#include <gpcl/detail/config.hpp>

#include <tuple>
#include <type_traits>

namespace gpcl::detail {
template <typename Tuple, typename Callable>
void tuple_for_each_impl(Tuple &&, Callable &&, std::index_sequence<>)
{
}

template <typename Tuple, typename Callable, std::size_t I, std::size_t... Is>
void tuple_for_each_impl(Tuple &&tuple, Callable &&callable,
                         std::index_sequence<I, Is...>)
{
  callable(std::get<I>(std::forward<Tuple>(tuple)));
  tuple_for_each_impl(std::forward<Tuple>(tuple),
                      std::forward<Callable>(callable),
                      std::index_sequence<Is...>{});
}

template <typename Tuple, typename Callable>
void tuple_for_each(Tuple &&tuple, Callable &&callable)
{
  tuple_for_each_impl(
      std::forward<Tuple>(tuple), std::forward<Callable>(callable),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}
} // namespace gpcl::detail

#endif // GPCL_DETAIL_TUPLE_FOR_EACH_HPP
