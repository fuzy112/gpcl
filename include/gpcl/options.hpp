//
// options.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OPTIONS_HPP
#define GPCL_OPTIONS_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {

inline namespace options {

template <typename T>
struct void_pointer
{
  using type = T;
};

template <typename T>
struct tag
{
  using type = T;
};

template <typename T>
struct compare
{
  using type = T;
};

template <typename T>
struct projection
{
  using type = T;
};

template <typename T>
struct base_hook
{
  using type = T;
};

template <typename E>
struct constant_time_size
{
  using type = E;
};

template <typename T>
struct link_mode
{
  using type = T;
};

using normal_link = std::integral_constant<int, 0>;
using safe_link = std::integral_constant<int, 1>;
using auto_unlink = std::integral_constant<int, 2>;

template <typename T>
struct value_traits
{
  using type = T;
};

template <typename T>
struct key_of_value
{
  using type = T;
};

template <bool V>
using constant_time_size_c = constant_time_size<std::bool_constant<V>>;

template <template <typename...> typename Template>
struct is_specialization_of
{
  template <typename... Ts>
  struct invoke
  {
    using type = std::false_type;
  };
};

template <template <typename...> typename Template>
template <typename... Ts>
struct is_specialization_of<Template>::invoke<Template<Ts...>>
{
  using type = std::true_type;
};

template <typename List, template <typename...> typename Option,
          typename Default>
using find_option = meta::at_c<
    meta::push_back<meta::filter<List, is_specialization_of<Option>>, Default>,
    0>;

} // namespace options

} // namespace gpcl

#endif // !GPCL_OPTIONS_HPP
