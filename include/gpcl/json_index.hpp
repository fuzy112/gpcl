//
// json_index.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_JSON_INDEX_HPP
#define GPCL_JSON_INDEX_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/json_error.hpp>
#include <gpcl/tag_invoke.hpp>

#include <type_traits>

namespace gpcl {
namespace detail {

template <typename R>
struct json_index_fn
{
  static_assert(std::is_reference_v<R>);

  template <typename T, typename A1,
            typename = std::enable_if_t<std::is_same_v<
                tag_invoke_result_t<json_index_fn<R>, T &, const A1 &>, R>>>
  R operator()(T &t, const A1 &a1) const
  {
    return gpcl::tag_invoke(*this, t, a1);
  }

  template <typename A1>
  auto operator()(const A1 &a1) const
  {
    return
        [a1](auto &t)
            -> std::enable_if_t<
                std::is_same_v<R,
                               decltype(gpcl::tag_invoke(
                                   std::declval<json_index_fn<R>>(), t, a1))>,
                R> { return gpcl::tag_invoke(json_index_fn<R>{}, t, a1); };
  }
};

template <typename R, typename T, typename A1, typename = R>
struct is_indexable : std::false_type
{
};

template <typename R, typename T, typename A1>
struct is_indexable<R, T, A1, decltype(std::declval<T &>()[std::declval<A1>()])>
    : std::true_type
{
};

template <typename R, typename T, typename A1,
          std::enable_if_t<is_indexable<R, T, A1>::value, int> = 0>
R json_index(T &x, const A1 &a1)
{
  return x[a1];
}

template <typename R, typename T, typename A1,
          std::enable_if_t<!is_indexable<R, T, A1>::value, int> = 0>
[[noreturn]] R json_index(T &, const A1 &)
{
  throw_json_error(json_errc::operation_not_supported, "json_index", true);
}

template <typename R, typename T, typename A1>
R tag_invoke(json_index_fn<R>, T &t, const A1 &a1)
{
  return json_index<R>(t, a1);
}

} // namespace detail

template <typename R>
using json_index_fn = detail::json_index_fn<R>;

template <typename R>
constexpr json_index_fn<R> json_index{};

} // namespace gpcl

#endif // GPCL_JSON_INDEX_HPP
