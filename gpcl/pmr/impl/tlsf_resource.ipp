//
// tlsf_resource.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_IMPL_TLSF_RESOURCE_IPP
#define GPCL_PMR_IMPL_TLSF_RESOURCE_IPP

#include <gpcl/pmr/tlsf_resource.hpp>

namespace gpcl {
namespace pmr {

tlsf_resource::tlsf_resource(void *user_provided_buffer, std::size_t size,
                             memory_resource *upstream)
    : pool_list_{nullptr},
      upstream_{upstream}
{
  GPCL_ASSERT(size > gpcl::detail::tlsf_size());
  tlsf_ = gpcl::detail::tlsf_create_with_pool(user_provided_buffer, size);
  if (!tlsf_)
  {
    GPCL_THROW(std::runtime_error(__func__));
  }
}

tlsf_resource::tlsf_resource(std::size_t initial_buffer,
                             memory_resource *upstream)
    : pool_list_{nullptr},
      upstream_{upstream}
{
  tlsf_ = gpcl::detail::tlsf_create(request_new_buffer(
      initial_buffer + gpcl::detail::tlsf_size() + sizeof(pool_base)));
  if (!tlsf_)
  {
    release();
    GPCL_THROW(std::runtime_error(__func__));
  }
}

tlsf_resource::tlsf_resource(memory_resource *upstream)
    : tlsf_resource(8152, upstream)
{
}

tlsf_resource::~tlsf_resource()
{
  release();
}

void tlsf_resource::release()
{
  while (pool_list_)
  {
    auto next = pool_list_->next;
    upstream_->deallocate(pool_list_, pool_list_->size, alignof(pool_base));
    pool_list_ = next;
  }
}

void *tlsf_resource::request_new_buffer(std::size_t bytes)
{
  pool_base *base =
      static_cast<pool_base *>(upstream_->allocate(bytes, alignof(pool_base)));
  if (!base)
  {
    GPCL_THROW(std::bad_alloc());
  }
  ::new (base) pool_base{bytes};
  base->next = pool_list_;
  pool_list_ = base;
  return base + 1;
}

void *tlsf_resource::alloc_from_buffer(std::size_t bytes, std::size_t alignment)
{
  return gpcl::detail::tlsf_memalign(tlsf_, alignment, bytes);
}

void tlsf_resource::request_from_upstream()
{
  void *mem = request_new_buffer(next_request_size_);
  if (!gpcl::detail::tlsf_add_pool(tlsf_, mem,
                                   next_request_size_ - sizeof(pool_base)))
  {
    GPCL_FATAL(EFAULT);
  }
  next_request_size_ *= 2;
}

void *tlsf_resource::do_allocate(std::size_t bytes, std::size_t alignment)
{
  void *ret;

  goto alloc;
  do
  {
    request_from_upstream();
  alloc:
    ret = alloc_from_buffer(bytes, alignment);
  } while (!ret);

  return ret;
}

void tlsf_resource::do_deallocate(void *p, std::size_t bytes,
                                  std::size_t alignment)
{
  (void)bytes;
  (void)alignment;
  gpcl::detail::tlsf_free(tlsf_, p);
}

} // namespace pmr
} // namespace gpcl


#endif // GPCL_PMR_IMPL_TLSF_RESOURCE_IPP
