//
// memory_mapped_region.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MEMORY_MAPPED_REGION_HPP
#define GPCL_MEMORY_MAPPED_REGION_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/unique_ptr.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/posix_memory_map.hpp>
#  define GPCL_MEMORY_MAPPED_REGION_SUPPORTED 1
#endif

namespace gpcl {

#if defined GPCL_POSIX
inline constexpr detail::posix_memory_map_impl memory_map{};
using memory_map_options = detail::posix_memory_map_options;
#endif

using offset_t = long;

#if defined GPCL_MEMORY_MAPPED_REGION_SUPPORTED

class memory_mapped_region
{
  std::pair<void *, std::size_t> p_{nullptr, 0};

public:
  /// Create a mapped region for a mappable object.
  /// @tparam MemoryMappable must model MemoryMappable.
  template <typename MemoryMappable>
  memory_mapped_region(const MemoryMappable &mappable, access_mode mode,
                       offset_t offset = 0, std::size_t size = 0,
                       const void *address = 0,
                       memory_map_options options = memory_map_options{})
  {
    map(mappable, mode, offset, size, address, options);
  }

  memory_mapped_region() noexcept = default;

  memory_mapped_region(memory_mapped_region &&other) noexcept { swap(other); }

  ~memory_mapped_region() { unmap(); }

  void swap(memory_mapped_region &other) noexcept
  {
    using gpcl::swap;
    swap(p_, other.p_);
  }

  template <typename MemoryMappable>
  void map(const MemoryMappable &mappable, access_mode mode,
           offset_t offset = 0, std::size_t size = 0, const void *address = 0,
           memory_map_options options = memory_map_options{})
  {
    error_code ec;
    map(mappable, mode, offset, size, address, options, ec);
    if (ec)
      GPCL_THROW(system_error(ec, __func__));
  }

  template <typename MemoryMappable>
  void map(const MemoryMappable &mappable, access_mode mode, offset_t offset,
           std::size_t size, const void *address, memory_map_options options,
           error_code &error)
  {
    GPCL_ASSERT(!is_mapped());
    p_ = memory_map(mappable, mode, offset, size, address, options, error);
  }

  void unmap() noexcept
  {
    if (is_mapped())
      memory_map.unmap(p_);
    p_ = {};
  }

  bool is_mapped() const noexcept { return !!address(); }

  void *address() const noexcept { return p_.first; }

  std::size_t size() const noexcept { return p_.second; }

  static std::size_t page_size() noexcept { return memory_map.page_size(); }
};

inline void swap(memory_mapped_region &x, memory_mapped_region &y) noexcept
{
  x.swap(y);
}

#endif

} // namespace gpcl

#endif // GPCL_MEMORY_MAPPED_REGION_HPP
