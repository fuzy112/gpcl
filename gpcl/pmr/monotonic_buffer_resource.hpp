//
// monotonic_buffer_resource.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_MONOTONIC_BUFFER_RESOURCE_HPP
#define GPCL_PMR_MONOTONIC_BUFFER_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/pmr/default_resource.hpp>
#include <gpcl/pmr/memory_resource.hpp>

namespace gpcl {
namespace pmr {

class monotonic_buffer_resource : public memory_resource
{
public:
  GPCL_DECL
  monotonic_buffer_resource(void *buffer, std::size_t size,
                            memory_resource *upstream = get_default_resource());

  GPCL_DECL explicit monotonic_buffer_resource(
      std::size_t initial_buffer,
      memory_resource *upstream = get_default_resource());

  explicit monotonic_buffer_resource(
      memory_resource *upstream = get_default_resource())
      : monotonic_buffer_resource(1024, upstream)
  {
  }

  GPCL_DECL ~monotonic_buffer_resource() override;

  GPCL_DECL void release();

  memory_resource *upstream_resource() const { return upstream_; }

private:
  struct block_info
  {
    void *ptr;
    std::size_t size;
    std::size_t alignment;
  };

  GPCL_DECL void *alloc_from_buffer(std::size_t bytes, std::size_t alignment);

  GPCL_DECL void request_from_upstream();

  GPCL_DECL void *do_allocate(std::size_t bytes,
                              std::size_t alignment) override;

  GPCL_DECL void do_deallocate(void *p, std::size_t bytes,
                               std::size_t alignment) override;

  GPCL_DECL bool
  do_is_equal(const memory_resource &other) const noexcept override;

  mutable_buffer buffer_;
  memory_resource *upstream_;
  std::size_t next_buffer_bytes_ = 1024;
  block_info block_list_{};
};

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_MONOTONIC_BUFFER_RESOURCE_HPP
