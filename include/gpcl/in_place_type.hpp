//
// in_place_type.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IN_PLACE_TYPE_HPP
#define GPCL_IN_PLACE_TYPE_HPP

namespace gpcl {

template <typename T>
struct in_place_type_t
{
};

template <typename T>
constexpr in_place_type_t<T> in_place_type{};

} // namespace gpcl

#endif // GPCL_IN_PLACE_TYPE_HPP
