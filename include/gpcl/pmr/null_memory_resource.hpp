//
// null_memory_resource.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_NULL_MEMORY_RESOURCE_HPP
#define GPCL_PMR_NULL_MEMORY_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/pmr/memory_resource.hpp>

namespace gpcl {
namespace pmr {

class memory_resource;

GPCL_DECL memory_resource *null_memory_resource() noexcept;

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_NULL_MEMORY_RESOURCE_HPP
