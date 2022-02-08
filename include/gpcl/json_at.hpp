//
// json_at.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_JSON_AT_HPP
#define GPCL_JSON_AT_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/tag_invoke.hpp>
#include <gpcl/json_error.hpp>

#include <type_traits>

namespace gpcl {

namespace detail {

template <typename R>
struct json_at_fn
{
  static_assert(std::is_reference_v<R>);

  template <typename T, typename A1,
            typename = std::enable_if_t<std::is_same_v<
                tag_invoke_result_t<json_at_fn<R>, T &, const A1 &>, R>>>
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
                std::is_same_v<R, decltype(gpcl::tag_invoke(
                                      std::declval<json_at_fn<R>>(), t, a1))>,
                R> { return gpcl::tag_invoke(json_at_fn<R>{}, t, a1); };
  }
};

template <typename R, typename T, typename I, typename = R>
struct is_at_indexable : std::false_type
{
};

template <typename R, typename T, typename I>
struct is_at_indexable<R, T, I,
                       decltype(std::declval<T &>().at(std::declval<I>()))>
    : std::true_type
{
};

template <typename R, typename T, typename A1,
          std::enable_if_t<is_at_indexable<R, T, A1>::value, int> = 0>
R json_at(T &x, A1 const &a1)
{
  return x.at(a1);
}

template <typename R, typename T, typename A1,
          std::enable_if_t<!is_at_indexable<R, T, A1>::value, int> = 0>
[[noreturn]] R json_at(T &, A1 const &)
{
  throw_json_error(json_errc::operation_not_supported, "json_at", true);
}

template <typename R, typename T, typename A1>
R tag_invoke(json_at_fn<R>, T &t, const A1 &a1)
{
  return json_at<R>(t, a1);
}

} // namespace detail

template <typename R>
using json_at_fn = detail::json_at_fn<R>;

template <typename R>
constexpr json_at_fn<R> json_at{};
} // namespace gpcl

#endif // GPCL_JSON_AT_HPP
