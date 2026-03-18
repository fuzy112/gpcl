//
// invoke.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_INVOKE_HPP
#define GPCL_INVOKE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/meta.hpp>

#include <functional>
#include <type_traits>

namespace gpcl {

template <typename F, typename... Args>
struct invoke_result;

template <typename F, typename... Args>
struct is_invocable;

namespace detail {

template <typename T>
struct get_class
{
};

template <typename C, typename T>
struct get_class<T C::*>
{
  using type = C;
};

template <typename C, typename R, typename... Args>
struct get_class<R (C::*)(Args...)>
{
  using type = C;
};

template <typename C, typename R, typename... Args>
struct get_class<R (C::*const)(Args...)>
{
  using type = C;
};

template <
    typename F, typename T,
    typename std::enable_if<
        std::is_base_of<typename get_class<typename std::decay<F>::type>::type,
                        typename std::decay<T>::type>::value,
        int>::type = 0>
constexpr T &&unwrap_ref(T &&x) noexcept
{
  return std::forward<T>(x);
}

template <typename F, typename T>
constexpr T &unwrap_ref(std::reference_wrapper<T> x) noexcept
{
  return x.get();
}

template <typename F, typename T>
constexpr T &unwrap_ref(T *x) noexcept
{
  return *x;
}

// invoke pointer to member function
template <typename F, typename T1, typename... Ts,
          typename std::enable_if<std::is_member_function_pointer<
                                      typename std::decay<F>::type>::value,
                                  int>::type = 0>
constexpr auto invoke_impl(F f, T1 &&t1, Ts &&...ts)
    -> decltype((detail::unwrap_ref<F>(std::forward<T1>(t1)).*
                 f)(std::forward<Ts>(ts)...))
{
  return (detail::unwrap_ref<F>(std::forward<T1>(t1)).*
          f)(std::forward<Ts>(ts)...);
}

// invoke pointer to member object
template <typename F, typename T1,
          typename std::enable_if<std::is_member_object_pointer<
                                      typename std::decay<F>::type>::value,
                                  int>::type = 0>
constexpr auto invoke_impl(F f, T1 &&t1)
    -> decltype(detail::unwrap_ref<F>(std::forward<T1>(t1)).*f)
{
  return detail::unwrap_ref<F>(std::forward<T1>(t1)).*f;
}

// invoke function object
template <typename F, typename... Ts>
constexpr auto invoke_impl(F &&f, Ts &&...ts)
    -> decltype(std::forward<F>(f)(std::forward<Ts>(ts)...))
{
  return std::forward<F>(f)(std::forward<Ts>(ts)...);
}

template <typename F, typename Args, typename = void>
struct is_invocable_impl : std::false_type
{
};

template <typename F, typename... Ts>
struct is_invocable_impl<F, meta::list<Ts...>,
                         decltype(detail::invoke_impl(std::declval<F>(),
                                                      std::declval<Ts>()...),
                                  void())> : std::true_type
{
};

template <typename F, typename Args, typename = void>
struct invoke_result_impl
{
};

template <typename F, typename... Ts>
struct invoke_result_impl<F, meta::list<Ts...>,
                          decltype((void)detail::invoke_impl(
                              std::declval<F>(), std::declval<Ts>()...))>
{
  using type =
      decltype(detail::invoke_impl(std::declval<F>(), std::declval<Ts>()...));
};

} // namespace detail

template <typename F, typename... Ts>
struct is_invocable : detail::is_invocable_impl<F, meta::list<Ts...>>
{
};
template <typename F, typename... Ts>
struct invoke_result : detail::invoke_result_impl<F, meta::list<Ts...>>
{
};

template <typename F, typename... Ts>
constexpr typename invoke_result<F, Ts...>::type invoke(F &&f, Ts &&...ts)
{
  return detail::invoke_impl(std::forward<F>(f), std::forward<Ts>(ts)...);
}

template <typename F, typename ...Ts>
using invoke_result_t = typename invoke_result<F, Ts...>::type;

} // namespace gpcl

#endif // GPCL_INVOKE_HPP
