//
// shared_block_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_SHARED_BLOCK_BASE_HPP
#define GPCL_DETAIL_SHARED_BLOCK_BASE_HPP

#include <gpcl/detail/config.hpp>

#ifdef _MSC_VER
#  include <gpcl/detail/msvc_shared_block_base.hpp>
#elif defined(__GNUC__)
#  include <gpcl/detail/gcc_shared_block_base.hpp>
#endif

namespace gpcl {
namespace detail {

#ifdef _MSC_VER
typedef msvc_shared_block_base shared_block_base;
#elif defined(__GNUC__)
typedef gcc_shared_block_base shared_block_base;
#endif

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_SHARED_BLOCK_BASE_HPP
