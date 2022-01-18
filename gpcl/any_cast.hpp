//
// any_cast.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ANY_CAST_HPP
#define GPCL_ANY_CAST_HPP

#include <gpcl/bad_any_cast.hpp>
#include <gpcl/basic_any.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>

#include <cstdlib>

namespace gpcl {

/// @name any_cast
/// @brief Type-safe access to the contained object
/// @relates gpcl::any
/// @relates gpcl::basic_any
/// @{

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
const T *any_cast_unchecked(const basic_any<LocalSize, LocalAlign> *x)
{
  GPCL_ASSERT(x != nullptr);
  return static_cast<const T *>(x->raw_value());
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
T *any_cast_unchecked(basic_any<LocalSize, LocalAlign> *x)
{
  GPCL_ASSERT(x != nullptr);
  return static_cast<T *>(x->raw_value());
}

#if defined GPCL_NO_RTTI
#  define GPCL_DETAIL_DEPRECATED_IF_NO_RTTI [[deprecated("Using any_cast when RTTI disabled is deprecated.")]]
#else
#  define GPCL_DETAIL_DEPRECATED_IF_NO_RTTI
#endif

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>

T any_cast(const basic_any<LocalSize, LocalAlign> &x)
{
  if (!holds_type<T>(x))
    GPCL_THROW(bad_any_cast());
  return *any_cast_unchecked<T>(&x);
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
GPCL_DETAIL_DEPRECATED_IF_NO_RTTI
T any_cast(basic_any<LocalSize, LocalAlign> &x)
{
  if (!holds_type<T>(x))
    GPCL_THROW(bad_any_cast());
  return *any_cast_unchecked<T>(&x);
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
GPCL_DETAIL_DEPRECATED_IF_NO_RTTI
T any_cast(basic_any<LocalSize, LocalAlign> &&x)
{
  if (!holds_type<T>(x))
    GPCL_THROW(bad_any_cast());
  return std::move(*any_cast_unchecked<T>(&x));
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
GPCL_DETAIL_DEPRECATED_IF_NO_RTTI
const T *
any_cast(const basic_any<LocalSize, LocalAlign> *x) noexcept
{
  if (!x)
    return nullptr;

  if (!holds_type<T>(*x))
    return nullptr;

  return any_cast_unchecked<T>(x);
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
GPCL_DETAIL_DEPRECATED_IF_NO_RTTI
T *any_cast(basic_any<LocalSize, LocalAlign> *x) noexcept
{
  if (!x)
    return nullptr;

  if (!holds_type<T>(*x))
    return nullptr;

  return any_cast_unchecked<T>(x);
}

#undef GPCL_DETAIL_DEPRECATED_IF_NO_RTTI

/// @}

} // namespace gpcl

#endif // GPCL_ANY_CAST_HPP
