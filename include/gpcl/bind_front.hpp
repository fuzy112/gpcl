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

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>

#include <functional>
#include <tuple>

namespace gpcl {

#if __cpp_init_captures >= 201803

template <typename Fn, typename... Ts>
auto bind_front(Fn &&fn, Ts &&...xs)
{
  return [fn = static_cast<Fn &&>(fn), ... xs = static_cast<Ts &&>(xs)](
             auto &&...ys) mutable -> decltype(auto) {
    return fn(xs..., static_cast<decltype(ys) &&>(ys)...);
  };
}

#elif defined(__cpp_lib_apply)

template <typename Fn, typename... Ts>
auto bind_front(Fn &&fn, Ts &&...xs)
{
  return [fn = static_cast<Fn &&>(fn),
          xs = std::make_tuple(std::forward<Ts>(xs)...)](
             auto &&...ys) mutable -> decltype(auto) {
    return std::apply(fn, std::tuple_cat(xs, std::forward_as_tuple(ys...)));
  };
}

#endif

} // namespace gpcl

#endif // GPCL_BIND_FRONT_HPP
