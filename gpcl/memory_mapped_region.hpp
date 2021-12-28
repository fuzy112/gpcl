//
// memory_mapped_region.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MEMORY_MAPPED_REGION_HPP
#define GPCL_MEMORY_MAPPED_REGION_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/unique_ptr.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/posix_memory_map.hpp>
#endif

namespace gpcl {

#if defined GPCL_POSIX
inline constexpr detail::posix_memory_map_impl memory_map{};
using memory_map_options = detail::posix_memory_map_options;
#endif

using offset_t = long;

class memory_mapped_region
{
  std::pair<void *, std::size_t> p_;

public:
  /// Create a mapped region for a mappable object.
  /// @tparam MemoryMappable must model MemoryMappable.
  template <typename MemoryMappable>
  memory_mapped_region(const MemoryMappable &mappable, access_mode mode,
                       offset_t offset = 0, std::size_t size = 0,
                       const void *address = 0,
                       memory_map_options options = memory_map_options{})
      : p_(memory_map(mappable, mode, offset, size, address, options))
  {
  }

  memory_mapped_region() noexcept = default;

  memory_mapped_region(memory_mapped_region &&other) noexcept { swap(other); }

  ~memory_mapped_region() { memory_map.unmap(p_); }

  void swap(memory_mapped_region &other) noexcept { gpcl::swap(p_, other.p_); }

  void *address() const noexcept { return p_.first; }

  std::size_t size() const noexcept { return p_.second; }

  static std::size_t page_size() noexcept { return memory_map.page_size(); }
};

namespace swap_detail {
inline void swap(memory_mapped_region &x, memory_mapped_region &y) noexcept
{
  x.swap(y);
}
} // namespace swap_detail

} // namespace gpcl

#endif // GPCL_MEMORY_MAPPED_REGION_HPP
