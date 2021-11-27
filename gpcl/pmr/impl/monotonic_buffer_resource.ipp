//
// monotonic_buffer_resource.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_IMPL_MONOTONIC_BUFFER_RESOURCE_IPP
#define GPCL_PMR_IMPL_MONOTONIC_BUFFER_RESOURCE_IPP

#include <gpcl/pmr/default_resource.hpp>
#include <gpcl/pmr/monotonic_buffer_resource.hpp>

namespace gpcl {
namespace pmr {

monotonic_buffer_resource::monotonic_buffer_resource(void *buffer,
                                                     std::size_t size,
                                                     memory_resource *upstream)
    : buffer_(reinterpret_cast<char *>(buffer), size),
      upstream_(upstream)
{
  GPCL_ASSERT(buffer);
  GPCL_ASSERT(upstream);
}

monotonic_buffer_resource::monotonic_buffer_resource(std::size_t initial_buffer,
                                                     memory_resource *upstream)
    : upstream_(upstream),
      next_buffer_bytes_(initial_buffer)
{
  GPCL_ASSERT(initial_buffer != 0);
  GPCL_ASSERT(upstream);
  request_from_upstream();
}

void monotonic_buffer_resource::release()
{
  while (block_list_.ptr)
  {
    block_info next;
    std::memcpy(&next,
                reinterpret_cast<char *>(block_list_.ptr) + block_list_.size,
                sizeof(next));
    upstream_->deallocate(block_list_.ptr,
                          block_list_.size + sizeof(block_info),
                          block_list_.alignment);
    block_list_ = next;
  }

  GPCL_VERIFY(block_list_.ptr == nullptr);
  GPCL_VERIFY(block_list_.size == 0);
}

void *monotonic_buffer_resource::alloc_from_buffer(std::size_t bytes,
                                                   std::size_t alignment)
{
  std::uintptr_t off =
      reinterpret_cast<std::uintptr_t>(buffer_.data()) % alignment;
  off = (alignment - off) % alignment;

  if (off > buffer_.size_bytes())
    return nullptr;
  buffer_ += off;
  if (buffer_.size_bytes() < bytes)
    return nullptr;
  auto *ret = buffer_.data();
  buffer_ += bytes;
  return ret;
}

void monotonic_buffer_resource::request_from_upstream()
{
  auto *p = upstream_->allocate(next_buffer_bytes_ + sizeof(block_info));
  std::memcpy(reinterpret_cast<char *>(p) + next_buffer_bytes_, &block_list_,
              sizeof block_list_);
  block_list_.ptr = p;
  block_list_.size = next_buffer_bytes_;
  block_list_.alignment = alignof(std::max_align_t);
  buffer_ = buffer(p, next_buffer_bytes_);
  next_buffer_bytes_ *= 2;
}

void *monotonic_buffer_resource::do_allocate(std::size_t bytes,
                                             std::size_t alignment)
{
  if (auto ret = alloc_from_buffer(bytes, alignment))
    return ret;

  if (next_buffer_bytes_ < bytes)
    next_buffer_bytes_ = (bytes + sizeof(std::max_align_t) - 1) /
                         sizeof(std::max_align_t) * sizeof(std::max_align_t);

  request_from_upstream();

  return alloc_from_buffer(bytes, alignment);
}

void monotonic_buffer_resource::do_deallocate(void *p, std::size_t bytes,
                                              std::size_t alignment)
{
  (void)p;
  (void)bytes;
  (void)alignment;
}

bool monotonic_buffer_resource::do_is_equal(
    const memory_resource &other) const noexcept
{
  return &other == this;
}

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_IMPL_MONOTONIC_BUFFER_RESOURCE_IPP
