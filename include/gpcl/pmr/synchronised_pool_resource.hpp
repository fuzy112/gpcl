//
// synchronised_pool_resource.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_SYNCHRONISED_POOL_RESOURCE_HPP
#define GPCL_PMR_SYNCHRONISED_POOL_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/pmr/default_resource.hpp>
#include <gpcl/pmr/detail/resource_pool.hpp>
#include <gpcl/pmr/memory_resource.hpp>
#include <gpcl/pmr/polymorphic_allocator.hpp>
#include <gpcl/pmr/pool_options.hpp>
#include <gpcl/scoped_lock.hpp>

#include <map>
#include <set>

namespace gpcl::pmr {

class synchronised_pool_resource : public memory_resource
{
  using pool_type = pmr::detail::resource_pool;

  const pool_options options_;

  memory_resource *const upstream_;

  mutex mtx_;

  using allocation_result = std::tuple<void *, std::size_t, std::size_t>;

  std::set<allocation_result, std::less<allocation_result>, polymorphic_allocator<allocation_result>>
      blocks_from_upstream_;

  std::map<std::size_t, pool_type, std::less<std::size_t>,
           polymorphic_allocator<std::pair<const std::size_t, pool_type>>>
      pools_;

public:
  synchronised_pool_resource()
      : synchronised_pool_resource(get_default_resource())
  {
  }

  explicit synchronised_pool_resource(memory_resource *upstream)
      : synchronised_pool_resource(pool_options{}, upstream)
  {
  }

  explicit synchronised_pool_resource(pool_options const &opts)
      : synchronised_pool_resource(opts, get_default_resource())
  {
  }

  synchronised_pool_resource(pool_options const &opts,
                             memory_resource *upstream)
      : options_(opts),
        upstream_(upstream),
        blocks_from_upstream_(upstream),
        pools_(upstream)
  {
    create_pools();
  }

  synchronised_pool_resource(const synchronised_pool_resource &) = delete;

  ~synchronised_pool_resource()
  {
    for (auto block : blocks_from_upstream_)
    {
      upstream_->deallocate(std::get<0>(block), std::get<1>(block),
                            std::get<2>(block));
    }
  }

  void release() noexcept
  {
    scoped_lock lock(mtx_);
    pools_.clear();

    for (auto block : blocks_from_upstream_)
    {
      upstream_->deallocate(std::get<0>(block), std::get<1>(block),
                            std::get<2>(block));
    }
    blocks_from_upstream_.clear();

    create_pools();
  }

  memory_resource *upstream_resource() const { return upstream_; }

  pool_options options() const { return options_; }

protected:
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    if (bytes >= options_.largest_required_pool_block ||
        alignment > alignof(std::max_align_t))
    {
      return upstream_->allocate(bytes, alignment);
    }

    scoped_lock lock(mtx_);
    return allocate_from_pool(bytes, alignment);
  }

  void do_deallocate(void *ptr, std::size_t bytes,
                     std::size_t alignment) noexcept override
  {
    if (bytes >= options_.largest_required_pool_block ||
        alignment > alignof(std::max_align_t))
    {
      return upstream_->deallocate(ptr, bytes, alignment);
    }

    scoped_lock lock(mtx_);
    deallocate_from_pool(ptr, bytes, alignment);
  }

  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return this == &other;
  }

  void *allocate_from_pool(std::size_t bytes, std::size_t alignment)
  {
    pool_type &pool = get_pool(bytes, alignment);
    GPCL_ASSERT(bytes % pool.requested_size() == 0);
    return pool.ordered_malloc(bytes / pool.requested_size());
  }

  void deallocate_from_pool(void *ptr, std::size_t bytes,
                            std::size_t alignment) noexcept
  {
    pool_type &pool = get_pool(bytes, alignment);
    GPCL_ASSERT(bytes % pool.requested_size() == 0);
    pool.ordered_free(ptr, bytes / pool.requested_size());
  }

  pool_type &get_pool(std::size_t bytes, std::size_t alignment) noexcept
  {
    (void)alignment;
    for (auto iter = pools_.rbegin(), end = pools_.rend(); iter != end; ++iter)
    {
      if (bytes % iter->first == 0)
      {
        return iter->second;
      }
    }
    GPCL_UNREACHABLE("impossible");
  }

  void create_pools()
  {
    std::size_t pool_sizes[] = {
        1, 2, 4, 8, 16, 32, 64, 96, 128, 256,
    };

    for (auto const size : pool_sizes)
    {
      if (size >= options_.largest_required_pool_block)
        break;
      pools_.try_emplace(size, size, upstream_);
    }
  }
};
} // namespace gpcl::pmr

#endif // GPCL_PMR_SYNCHRONISED_POOL_RESOURCE_HPP
