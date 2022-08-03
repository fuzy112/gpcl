//
// pool_options.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_POOL_OPTIONS_HPP
#define GPCL_PMR_POOL_OPTIONS_HPP

#include <gpcl/detail/config.hpp>

#include <cstddef>

namespace gpcl::pmr {

struct pool_options
{
  std::size_t max_blocks_per_chunk = 0;

  std::size_t largest_required_pool_block = 128;
};

} // namespace gpcl::pmr

#endif
