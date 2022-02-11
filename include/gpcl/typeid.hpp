//
// typeid.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_TYPEID_HPP
#define GPCL_TYPEID_HPP


#include <gpcl/detail/config.hpp>

#if defined(GPCL_CONFIG_NO_RTTI)
#  include <gpcl/detail/typeid.hpp>
#else
#  include <typeinfo>
#endif

#include <type_traits>

namespace gpcl {

#if defined(GPCL_CONFIG_NO_RTTI)
using type_info = detail::type_info;

template <typename T>
constexpr const type_info &typeid_() noexcept
{
  return detail::typeid_impl<T>::typeid_;
}

template <typename T, typename = type_info>
struct has_type_info : std::false_type
{
};

template <typename T>
struct has_type_info<
    T, typename std::decay<decltype(std::declval<const T &>().type())>::type>
    : std::true_type
{
};

template <typename T, typename = bool>
struct has_is_a : std::false_type
{
};

template <typename T>
struct has_is_a<T, typename std::decay<decltype(std::declval<const T &>().is_a(
                       std::declval<type_info const &>()))>::type>
    : std::true_type
{
};

template <typename T, typename U,
          typename std::enable_if<has_is_a<U>::value, int>::type = 0>
bool is_a(const U *u) noexcept
{
  if (!u)
    return false;
  return u->is_a(typeid_<T>());
}

template <typename T, typename U,
          typename std::enable_if<!has_is_a<U>::value, int>::type = 0>
bool is_a(const U *) noexcept
{
  return std::is_base_of<T, U>::value;
}

template <typename T,
          typename std::enable_if<has_type_info<T>::value, int>::type = 0>
const type_info &typeid_(const T &v) noexcept
{
  return v.type();
}

template <typename T, typename U,
          typename std::enable_if<has_type_info<U>::value, int>::type = 0>
T *dyn_cast(U *u) noexcept
{
  if (is_a<T>(u))
  {
    return static_cast<T *>(
        static_cast<typename std::common_type<T, U>::type *>(u));
  }
  return nullptr;
}

template <typename T, typename U,
          typename std::enable_if<has_type_info<U>::value, int>::type = 0>
const T *dyn_cast(const U *u) noexcept
{
  if (is_a<T>(u))
  {
    return static_cast<const T *>(
        static_cast<const typename std::common_type<T, U>::type *>(u));
  }
  return nullptr;
}

template <typename T, typename U,
          typename std::enable_if<!has_type_info<U>::value &&
                                      std::is_base_of<T, U>::value,
                                  int>::type = 0>
T *dyn_cast(U *u...) noexcept
{
  return static_cast<T *>(u);
}

template <typename T, typename U,
          typename std::enable_if<!has_type_info<U>::value &&
                                      std::is_base_of<T, U>::value,
                                  int>::type = 0>
const T *dyn_cast(const U *u...) noexcept
{
  return static_cast<const T *>(u);
}

template <typename T, typename U,
          typename std::enable_if<!has_type_info<U>::value &&
                                      !std::is_base_of<T, U>::value,
                                  int>::type = 0>
T *dyn_cast(U *u...) noexcept
{
  return nullptr;
}

template <typename T, typename U,
          typename std::enable_if<!has_type_info<U>::value &&
                                      !std::is_base_of<T, U>::value,
                                  int>::type = 0>
const T *dyn_cast(const U *u...) noexcept
{
  return nullptr;
}

#else

using type_info = std::type_info;

template <typename T>
constexpr const type_info &typeid_() noexcept
{
  return typeid(T);
}

template <typename T>
struct has_type_info : std::true_type
{
};

template <typename T>
const type_info &typeid_(const T *v) noexcept
{
  return typeid(v);
}

/// @todo Use static_cast when T is base of U
template <typename T, typename U>
T *dyn_cast(U *u) noexcept
{
  return dynamic_cast<T *>(u);
}

/// @todo Use static_cast when T is base of U
template <typename T, typename U>
const T *dyn_cast(const U *u) noexcept
{
  return dynamic_cast<const T *>(u);
}

template <typename T, typename U>
bool is_a(const U *u) noexcept
{
  return dynamic_cast<const T *>(u) != nullptr;
}

#endif

} // namespace gpcl


#endif // GPCL_TYPEID_HPP
