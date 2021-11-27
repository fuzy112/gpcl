//
// tlsf_resource.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_TLSF_RESOURCE_HPP
#define GPCL_PMR_TLSF_RESOURCE_HPP

#include <gpcl/detail/tlsf.hpp>
#include <gpcl/intrusive_list.hpp>
#include <gpcl/pmr/default_resource.hpp>
#include <gpcl/pmr/memory_resource.hpp>

namespace gpcl {
namespace pmr {

class tlsf_resource : public memory_resource
{
public:
  GPCL_DECL tlsf_resource(void *buffer, std::size_t size,
                          memory_resource *upstream = get_default_resource());

  GPCL_DECL explicit tlsf_resource(
      std::size_t initial_buffer,
      memory_resource *upstream = get_default_resource());

  explicit tlsf_resource(memory_resource *upstream = get_default_resource())
      : tlsf_resource(8152, upstream)
  {
  }

  GPCL_DECL ~tlsf_resource() override;

  memory_resource *upstream_resource() const { return upstream_; }

private:
  GPCL_DECL void release();

  GPCL_DECL void *alloc_from_buffer(std::size_t bytes, std::size_t alignment);

  GPCL_DECL void *request_new_buffer(std::size_t bytes);
  GPCL_DECL void request_from_upstream();

  GPCL_DECL void *do_allocate(std::size_t bytes,
                              std::size_t alignment) override;

  GPCL_DECL void do_deallocate(void *p, std::size_t bytes,
                               std::size_t alignment) override;

  GPCL_DECL bool
  do_is_equal(const memory_resource &other) const noexcept override;

  struct pool_base
  {
    explicit pool_base(std::size_t size) : size(size), next(nullptr) {}

    std::size_t size;
    pool_base *next;
  };

  gpcl::detail::tlsf_t tlsf_;
  pool_base *pool_list_;
  memory_resource *upstream_;
  std::size_t next_request_size_ = 8152;
};

} // namespace pmr
} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/pmr/impl/tlsf_resource.ipp>
#endif

#endif // GPCL_PMR_TLSF_RESOURCE_HPP
