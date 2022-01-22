#ifndef GPCL_PMR_POOL_OPTIONS_HPP
#define GPCL_PMR_POOL_OPTIONS_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl::pmr {

struct pool_options
{
  std::size_t max_blocks_per_chunk = 0;

  std::size_t largest_required_pool_block = 128;
};

} // namespace gpcl::pmr

#endif
