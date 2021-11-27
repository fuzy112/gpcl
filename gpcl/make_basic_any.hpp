//
// make_basic_any.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MAKE_BASIC_ANY_HPP
#define GPCL_MAKE_BASIC_ANY_HPP

#include <gpcl/basic_any.hpp>
#include <gpcl/in_place_type.hpp>

#include <initializer_list>
#include <utility>

namespace gpcl {
template <typename BasicAnySpecialization, typename T, typename... Args>
BasicAnySpecialization make_basic_any(Args &&... args)
{
  static_assert(is_basic_any_v<BasicAnySpecialization>);
  return BasicAnySpecialization(in_place_type<T>, std::forward<Args>(args)...);
}

template <typename BasicAnySpecialization, typename T, typename U,
          typename... Args>
BasicAnySpecialization make_basic_any(std::initializer_list<U> il,
                                      Args &&... args)
{
  static_assert(is_basic_any_v<BasicAnySpecialization>);
  return BasicAnySpecialization(in_place_type<T>, il,
                                std::forward<Args>(args)...);
}
} // namespace gpcl

#endif // GPCL_MAKE_BASIC_ANY_HPP
