//
// shared_block_operation.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_SHARED_BLOCK_OPERATION_HPP
#define GPCL_DETAIL_SHARED_BLOCK_OPERATION_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {
namespace detail {
enum shared_block_operation_t
{
  delete_control_block = 1,
  destroy_managed_object = 2,
};
}
} // namespace gpcl

#endif // GPCL_DETAIL_SHARED_BLOCK_OPERATION_HPP
