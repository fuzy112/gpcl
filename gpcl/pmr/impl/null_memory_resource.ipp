//
// null_memory_resource.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_IMPL_NULL_MEMORY_RESOURCE_IPP
#define GPCL_PMR_IMPL_NULL_MEMORY_RESOURCE_IPP

#include <gpcl/pmr/memory_resource.hpp>
#include <gpcl/pmr/null_memory_resource.hpp>

namespace gpcl {
namespace pmr {

namespace pmr_detail {
class null_memory_resource_impl : public memory_resource
{
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    (void)bytes;
    (void)alignment;
    GPCL_THROW(std::bad_alloc());
  }

  void do_deallocate(void *p, std::size_t bytes,
                     std::size_t alignment) noexcept override
  {
    (void)p;
    (void)bytes;
    (void)alignment;
  }

  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return &other == this;
  }
};
} // namespace pmr_detail

memory_resource *null_memory_resource() noexcept
{
  static pmr_detail::null_memory_resource_impl instance_;
  return &instance_;
}

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_IMPL_NULL_MEMORY_RESOURCE_IPP
