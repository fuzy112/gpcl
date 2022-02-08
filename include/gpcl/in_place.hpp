//
// in_place_t.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IN_PLACE_HPP
#define GPCL_IN_PLACE_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

struct in_place_t
{
};

inline constexpr in_place_t in_place{};


} // namespace gpcl

#endif // GPCL_IN_PLACE_HPP
