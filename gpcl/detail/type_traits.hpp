//
// type_traits.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_TYPE_TRAITS_HPP
#define GPCL_DETAIL_TYPE_TRAITS_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <type_traits>

namespace gpcl {
namespace detail {

#define GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(name_part)                        \
  template <typename T, bool = std::is_void<T>::value>                         \
  struct name_part;                                                            \
  template <typename T>                                                        \
  struct name_part<T, true> : std::false_type                                  \
  {                                                                            \
  };                                                                           \
  template <typename T>                                                        \
  struct name_part<T, false> : std::name_part<T>                               \
  {                                                                            \
  };                                                                           \
  template <typename T>                                                        \
  GPCL_CXX17_INLINE_CONSTEXPR bool name_part##_v = name_part<T>{};

GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_default_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_default_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_default_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_copy_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_copy_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_copy_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_move_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_move_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_move_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_copy_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_copy_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_copy_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_move_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_move_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_move_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_destructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_destructible)

#undef GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE

template <typename T>
struct is_char_like_type : std::false_type
{
};

template <>
struct is_char_like_type<char> : std::true_type
{
};

template <>
struct is_char_like_type<unsigned char> : std::true_type
{
};

template <>
struct is_char_like_type<signed char> : std::true_type
{
};

template <>
struct is_char_like_type<wchar_t> : std::true_type
{
};

template <>
struct is_char_like_type<char16_t> : std::true_type
{
};

template <>
struct is_char_like_type<char32_t> : std::true_type
{
};

template <typename T>
GPCL_CXX17_INLINE_CONSTEXPR bool is_char_like_type_v = is_char_like_type<T>{};

template <typename T, typename... Args>
GPCL_CXX17_INLINE_CONSTEXPR bool is_constructible_v =
    std::is_constructible<T, Args...>{};

template <typename T, typename... Args>
GPCL_CXX17_INLINE_CONSTEXPR bool is_nothrow_constructible_v =
    std::is_nothrow_constructible<T, Args...>{};

template <typename T>
GPCL_CXX17_INLINE_CONSTEXPR bool is_void_v = std::is_void<T>{};

template <typename T, typename U>
GPCL_CXX17_INLINE_CONSTEXPR bool is_same_v = std::is_same<T, U>{};

template <typename From, typename To>
GPCL_CXX17_INLINE_CONSTEXPR bool is_convertible_v =
    std::is_convertible<From, To>{};

template <typename T, typename U>
GPCL_CXX17_INLINE_CONSTEXPR bool is_assignable_v = std::is_assignable<T, U>{};

template <bool... Bs>
struct conjunction_helper;

template <>
struct conjunction_helper<> : std::true_type
{
};

template <bool... Bs>
struct conjunction_helper<false, Bs...> : std::false_type
{
};

template <bool... Bs>
struct conjunction_helper<true, Bs...> : conjunction_helper<Bs...>
{
};

template <typename... Ts>
using conjunction = conjunction_helper<Ts::value...>;

template <typename... Ts>
GPCL_CXX17_INLINE_CONSTEXPR bool conjunction_v = conjunction<Ts...>::value;

template <bool... Bs>
struct disjunction_helper;

template <>
struct disjunction_helper<> : std::false_type
{
};

template <bool... Bs>
struct disjunction_helper<true, Bs...> : std::true_type
{
};

template <bool... Bs>
struct disjunction_helper<false, Bs...> : disjunction_helper<Bs...>
{
};

template <typename... Ts>
using disjunction = disjunction_helper<Ts::value...>;

template <typename... Ts>
GPCL_CXX17_INLINE_CONSTEXPR bool disjunction_v = disjunction<Ts...>::value;

template <typename T>
using negate = std::integral_constant<bool, !T::value>;

template <typename T>
GPCL_CXX17_INLINE_CONSTEXPR bool negate_v = negate<T>::value;

template <bool C, typename T = void>
using enable_if_t = typename std::enable_if<C, T>::type;

template <typename...>
using void_t = void;

template <typename T, typename U = T, typename = void>
struct is_swappable : std::false_type
{
};

template <typename T, typename U>
struct is_swappable<
    T, U, void_t<decltype(swap(std::declval<T &>(), std::declval<U &>()))>>
    : std::true_type
{
};

template <typename T, typename U = T, bool = is_swappable<T, U>::value>
struct is_nothrow_swappable;

template <typename T, typename U>
struct is_nothrow_swappable<T, U, false> : std::false_type
{
};

template <typename T, typename U>
struct is_nothrow_swappable<T, U, true>
    : std::integral_constant<bool, noexcept(swap(std::declval<T &>(),
                                                 std::declval<U &>()))>
{
};

template <class T>
struct is_unbounded_array : std::false_type
{
};

template <class T>
struct is_unbounded_array<T[]> : std::true_type
{
};

} // namespace detail

} // namespace gpcl

#endif // GPCL_TYPE_TRAITS_HPP
