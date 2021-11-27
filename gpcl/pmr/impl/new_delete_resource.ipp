//
// new_delete_resource.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_IMPL_NEW_DELETE_RESOURCE_IPP
#define GPCL_PMR_IMPL_NEW_DELETE_RESOURCE_IPP

#include <gpcl/pmr/memory_resource.hpp>
#include <gpcl/pmr/new_delete_resource.hpp>
#include <new>

namespace gpcl {
namespace pmr {

namespace pmr_detail {

class new_delete_resource_impl : public memory_resource
{
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    return ::operator new[](bytes, std::align_val_t(alignment));
  }
  void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override
  {
    (void)bytes;
    ::operator delete[](p, std::align_val_t(alignment));
  }
  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return &other == this;
  }
};

} // namespace pmr_detail

memory_resource *new_delete_resource() noexcept
{
  static pmr_detail::new_delete_resource_impl instance;
  return &instance;
}

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_IMPL_NEW_DELETE_RESOURCE_IPP
