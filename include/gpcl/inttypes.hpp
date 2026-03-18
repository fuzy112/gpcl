//
// inttypes.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_INTTYPES_HPP
#define GPCL_INTTYPES_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <cstdint>

namespace gpcl {
inline namespace inttypes {

using u8 = std::uint8_t;
using i8 = std::int8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

} // namespace inttypes
} // namespace gpcl

#endif // GPCL_INTTYPES_HPP
