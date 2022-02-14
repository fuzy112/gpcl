//
// vector.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2021-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_VECTOR_HPP
#define GPCL_VECTOR_HPP

#include <gpcl/array.hpp>
#include <gpcl/detail/config.hpp>

namespace gpcl {

template <typename T, typename Allocator = default_allocator<T>>
using vector = array<T, Allocator>;

} // namespace gpcl

#endif // GPCL_VECTOR_HPP
