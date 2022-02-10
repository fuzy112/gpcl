//
// bind_front.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BIND_FRONT_HPP
#define GPCL_BIND_FRONT_HPP

#include <gpcl/apply.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/invoke.hpp>

#include <tuple>

namespace gpcl {

namespace detail {
template <typename Fn, typename... Xs>
struct bind_expr
{
  Fn fn_;
  std::tuple<Xs...> xs_;

  template <typename... Ys>
  typename invoke_result<const Fn, Xs..., Ys &&...>::type
  operator()(Ys &&...ys) const
  {
    return gpcl::apply(fn_, std::tuple_cat(xs_, std::forward_as_tuple(ys...)));
  }

  template <typename... Ys>
  typename invoke_result<Fn, Xs..., Ys &&...>::type operator()(Ys &&...ys)
  {
    return gpcl::apply(fn_, std::tuple_cat(xs_, std::forward_as_tuple(ys...)));
  }
};
} // namespace detail

template <typename Fn, typename... Ts>
auto bind_front(Fn &&fn, Ts &&...ts)
{
  return detail::bind_expr<typename std::decay<Fn>::type,
                           typename std::decay<Ts>::type...>{
      std::forward<Fn>(fn),
      std::make_tuple(std::forward<Ts>(ts)...),
  };
}

} // namespace gpcl

#endif // GPCL_BIND_FRONT_HPP
