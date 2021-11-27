//
// any_cast.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ANY_CAST_HPP
#define GPCL_ANY_CAST_HPP

#include <gpcl/bad_any_cast.hpp>
#include <gpcl/basic_any.hpp>
#include <gpcl/detail/config.hpp>

#include <cstdlib>

namespace gpcl {
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

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
T any_cast(const basic_any<LocalSize, LocalAlign> &x)
{
  if (!x.template has_type<T>())
    GPCL_THROW(bad_any_cast());
  return *any_cast_unchecked<T>(&x);
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
T any_cast(basic_any<LocalSize, LocalAlign> &x)
{
  if (!x.template has_type<T>())
    GPCL_THROW(bad_any_cast());
  return *any_cast_unchecked<T>(&x);
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
T any_cast(basic_any<LocalSize, LocalAlign> &&x)
{
  if (!x.template has_type<T>())
    GPCL_THROW(bad_any_cast());
  return std::move(*any_cast_unchecked<T>(&x));
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
const T *any_cast(const basic_any<LocalSize, LocalAlign> *x) noexcept
{
  if (!x)
    return nullptr;

  if (!x->template has_type<T>())
    return nullptr;

  return any_cast_unchecked<T>(x);
}

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
T *any_cast(basic_any<LocalSize, LocalAlign> *x) noexcept
{
  if (!x)
    return nullptr;

  if (!x->template has_type<T>())
    return nullptr;

  return any_cast_unchecked<T>(x);
}
} // namespace gpcl

#endif // GPCL_ANY_CAST_HPP
