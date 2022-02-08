//
// ref_count_operation.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_REF_COUNT_OPERATION_HPP
#define GPCL_DETAIL_REF_COUNT_OPERATION_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {
namespace detail {
enum ref_count_operation_t
{
  delete_control_block = 1,
  destroy_managed_object = 2,
  get_deleter = 3,
  get_deleter_type_info = 4,
};
}
} // namespace gpcl

#endif // GPCL_DETAIL_REF_COUNT_OPERATION_HPP
