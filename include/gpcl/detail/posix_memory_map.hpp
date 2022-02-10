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
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>

#include <sys/mman.h>
#include <unistd.h>

namespace gpcl::detail {

using offset_t = std::ptrdiff_t;

struct posix_memory_map_options
{
  int flags = 0;
};

template <typename MemoryMappable>
std::pair<void *, std::size_t>
memory_map(const MemoryMappable &mappable, access_mode mode, offset_t offset,
           std::size_t size, const void *address,
           posix_memory_map_options options, error_code &error)
{
  int prot;
  if (mode == access_mode::read_write)
    prot = PROT_READ | PROT_WRITE;
  else
    prot = PROT_READ;

  if (!size)
    size = mappable.size();

#if defined(GPCL_LINUX)
  options.flags |= MAP_SHARED_VALIDATE;
#else
  options.flags |= MAP_SHARED;
#endif

#if defined(GPCL_LINUX)
  void *ret = ::mmap64((void *)address, size, prot, options.flags,
                       mappable.native_handle(), offset);
#else
  void *ret = ::mmap((void *)address, size, prot, options.flags,
                     mappable.native_handle(), offset);
#endif
  if (ret == MAP_FAILED)
  {
    error = {errno, system_category()};
    return {};
  }
  error = {};
  return {ret, size};
}

struct posix_memory_map_impl
{
  template <typename MemoryMappable>
  std::pair<void *, std::size_t>
  operator()(const MemoryMappable &mappable, access_mode mode, offset_t offset,
             std::size_t size, const void *address,
             posix_memory_map_options options, error_code &error) const
  {
    return memory_map(mappable, mode, offset, size, address, options, error);
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
