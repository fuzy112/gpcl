//
// default_resource.ipp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_IMPL_DEFAULT_RESOURCE_IPP
#define GPCL_PMR_IMPL_DEFAULT_RESOURCE_IPP

#include <gpcl/pmr/default_resource.hpp>
#include <gpcl/pmr/new_delete_resource.hpp>

#include <atomic>

namespace gpcl {
namespace pmr {

namespace pmr_detail {

inline std::atomic<memory_resource *> default_memory_resource =
    pmr::new_delete_resource();

} // namespace pmr_detail

void set_default_resource(memory_resource *resource) noexcept
{
  pmr_detail::default_memory_resource.store(resource,
                                            std::memory_order_release);
}

memory_resource *get_default_resource() noexcept
{
  return pmr_detail::default_memory_resource.load(std::memory_order_acquire);
}

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_IMPL_DEFAULT_RESOURCE_IPP
