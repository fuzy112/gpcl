//
// resource_pool.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_DETAIL_RESOURCE_POOL_HPP
#define GPCL_PMR_DETAIL_RESOURCE_POOL_HPP


#include <gpcl/detail/config.hpp>
#include <gpcl/pmr/memory_resource.hpp>
#include <gpcl/simple_segregated_storage.hpp>

namespace gpcl {
namespace pmr {

namespace detail {

class resource_pool
{
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  /// Constructor.
  explicit resource_pool(size_type requested_size, memory_resource *upstream)
      : upstream_(upstream),
        requested_size_(requested_size),
        chunk_size_(std::lcm(requested_size_,
                             std::lcm(sizeof(void *), sizeof(size_type)))),
        block_list_(),
        next_size_(32)
  {
    GPCL_ASSERT(upstream != nullptr);
    GPCL_ASSERT(requested_size != 0);
  }

  /// Destructor.
  ///
  /// During destruction, any malloc'd but not free'd memory will be recycled.
  ~resource_pool() noexcept
  {
    block_info block = block_list_;
    while (block.ptr)
    {
      auto next = *reinterpret_cast<block_info *>(
          block.ptr + block_info_offset(block.size));
      upstream_->deallocate(block.ptr, block.size);
      block = next;
    }
  }

  /// Determines the requested size.
  size_type requested_size() const noexcept { return requested_size_; }

  /// Allocates memory for an object of size requested_size.
  [[nodiscard]] void *malloc()
  {
    if (storage_.empty())
    {
      if (request_new_block(chunk_size_))
        return storage_.malloc(chunk_size_);
      else
        return nullptr;
    }

    return storage_.malloc(chunk_size_);
  }

  /// Allocates memory for an object of size requested_size.
  [[nodiscard]] void *ordered_malloc()
  {
    if (storage_.empty())
    {
      if (request_new_block(chunk_size_, true))
        return storage_.malloc(chunk_size_);
      else
        return nullptr;
    }

    return storage_.malloc(chunk_size_);
  }

  /// Allocates memory for an array of n objects of size requested_size.
  [[nodiscard]] void *ordered_malloc(size_type n)
  {
    GPCL_ASSERT(n != 0);
    auto count = chunk_count(n);
    if (auto ret = storage_.malloc_n(count, chunk_size_))
      return ret;
    request_new_block(count * chunk_size_, true);

    return storage_.malloc_n(count, chunk_size_);
  }

  /// Frees memory for an object.
  void free(void *ptr)
  {
    storage_.free(ptr);
  }

  /// Frees memory for an object.
  void ordered_free(void *ptr)
  {
    storage_.ordered_free(ptr);
  }

  /// Frees memory for an array of objects.
  void ordered_free(void *ptr, size_type n)
  {
    storage_.ordered_free_n(ptr, chunk_count(n), chunk_size_);
  }

  // not thread-safe
  size_type get_next_size() const { return next_size_; }

  // not thread-safe
  void set_next_size(size_type next_size)
  {
    GPCL_ASSERT(next_size != 0);
    next_size_ = next_size;
  }

  bool is_from(void *) const noexcept
  {
    GPCL_UNIMPLEMENTED();
    return false;
  }

private:
  struct block_info
  {
    // Pointer to the start of the next block.
    char *ptr;

    // Size of the next block.
    size_type size;
  }
#ifdef __GNUC__
  __attribute__((may_alias))
#endif
  ;

  // Requests a block with at least min_size bytes for chunks.
  bool request_new_block(size_type min_size, bool ordered = false)
  {
    auto sz = next_size_ * chunk_size_;
    while (sz < min_size + sizeof(block_info) * 2)
      sz *= 2;

    next_size_ *= 2;

    char *block = reinterpret_cast<char *>(upstream_->allocate(sz));
    if (block == nullptr)
      return false;

    block_list_ = add_block(block, sz, block_list_, ordered);
    GPCL_VERIFY_FALSE(storage_.empty());
    return true;
  }

  block_info add_block(char *ptr, size_type sz, block_info end, bool ordered)
  {
    auto chunks_size = block_info_offset(sz);
    if (ordered)
      storage_.add_ordered_block(ptr, chunks_size, chunk_size_);
    else
      storage_.add_block(ptr, chunks_size, chunk_size_);
    *reinterpret_cast<block_info *>(ptr + chunks_size) = end;
    return {ptr, sz};
  }

  difference_type block_info_offset(size_type block_size)
  {
    return (block_size - sizeof(block_info)) / alignof(block_info) *
           alignof(block_info);
  }

  size_type chunk_count(size_type n)
  {
    const size_type array_size = n * requested_size_;
    const size_type ret = (array_size + chunk_size_ - 1) / chunk_size_;
    GPCL_VERIFY(ret * chunk_size_ >= n * requested_size_);
    return ret;
  }

  memory_resource *upstream_;
  const size_type requested_size_;
  const size_type chunk_size_;
  size_type next_size_;
  block_info block_list_;
  simple_segregated_storage<size_type> storage_;
};
} // namespace detail
} // namespace pmr
} // namespace gpcl


#endif // GPCL_PMR_DETAIL_RESOURCE_POOL_HPP
