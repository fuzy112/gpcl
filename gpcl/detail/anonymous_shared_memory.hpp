#ifndef GPCL_DETAIL_ANONYMOUS_SHARED_MEMORY_HPP
#define GPCL_DETAIL_ANONYMOUS_SHARED_MEMORY_HPP

#include <gpcl/access_mode.hpp>
#include <gpcl/detail/posix_memory_map.hpp>

#include <cstdint>

#include <sys/mman.h>

namespace gpcl::detail {

class anonymous_shared_memory_impl
{
public:
  explicit constexpr anonymous_shared_memory_impl(std::size_t size)
      : size_(size)
  {
  }

  constexpr std::size_t size() const noexcept { return size_; }

  using native_handle_type = int;

  constexpr int native_handle() const noexcept { return -1; }

private:
  std::size_t size_;
};

inline std::pair<void *, std::size_t>
memory_map(const anonymous_shared_memory_impl &mem, access_mode mode,
           std::ptrdiff_t offset, std::size_t size, const void *address,
           memory_map_options options)
{
  GPCL_ASSERT(offset == 0);
  int prot;
  if (mode == access_mode::read_write)
    prot = PROT_READ | PROT_WRITE;
  else
    prot = PROT_READ;

  if (!size)
    size = mem.size();

  options.flags |= MAP_SHARED | MAP_ANONYMOUS;

  void *ret = ::mmap64((void *)address, size, prot, options.flags, -1, 0);
  if (ret == MAP_FAILED)
    throw_system_error(__func__);
  return {ret, size};
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ANONYMOUS_SHARED_MEMORY_HPP
