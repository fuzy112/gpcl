//
// posix_memory_map.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_MEMORY_MAP_HPP
#define GPCL_DETAIL_POSIX_MEMORY_MAP_HPP

#include <gpcl/access_mode.hpp>
#include <gpcl/detail/error.hpp>

#include <sys/mman.h>
#include <unistd.h>

namespace gpcl::detail {

using offset_t = std::ptrdiff_t;

struct posix_memory_map_options
{
  int flags = 0;
};

template <typename MemoryMappable>
std::pair<void *, std::size_t> memory_map(const MemoryMappable &mappable,
                                          access_mode mode, offset_t offset,
                                          std::size_t size, const void *address,
                                          posix_memory_map_options options)
{
  int prot;
  if (mode == access_mode::read_write)
    prot = PROT_READ | PROT_WRITE;
  else
    prot = PROT_READ;

  if (!size)
    size = mappable.size();

  options.flags |= MAP_SHARED_VALIDATE;

  void *ret = ::mmap64((void *)address, size, prot, options.flags,
                       mappable.native_handle(), offset);
  if (ret == MAP_FAILED)
    throw_system_error(__func__);
  return {ret, size};
}

struct posix_memory_map_impl
{
  template <typename MemoryMappable>
  std::pair<void *, std::size_t>
  operator()(const MemoryMappable &mappable, access_mode mode, offset_t offset,
             std::size_t size, const void *address,
             posix_memory_map_options options) const
  {
    return memory_map(mappable, mode, offset, size, address, options);
  }

  inline static void unmap(const void *addr, std::size_t length) noexcept
  {
    GPCL_VERIFY_0(::munmap((void *)addr, length));
  }

  inline static void unmap(std::pair<void *, std::size_t> p) noexcept
  {
    unmap(p.first, p.second);
  }

  inline static std::size_t page_size() noexcept
  {
    return sysconf(_SC_PAGE_SIZE);
  }
};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_POSIX_MEMORY_MAP_HPP
