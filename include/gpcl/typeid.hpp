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

#else

using type_info = std::type_info;

template <typename T>
constexpr const type_info &typeid_() noexcept
{
  return typeid(T);
}

#endif

} // namespace gpcl

#endif // GPCL_TYPEID_HPP
