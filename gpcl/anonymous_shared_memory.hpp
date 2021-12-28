#ifndef GPCL_ANONYMOUS_SHARED_MEMORY_HPP
#define GPCL_ANONYMOUS_SHARED_MEMORY_HPP

#include <gpcl/detail/config.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/anonymous_shared_memory.hpp>
namespace gpcl {
inline auto anonymous_shared_memory(std::size_t size) noexcept
{
  return gpcl::detail::anonymous_shared_memory_impl{size};
}
} // namespace gpcl
#endif

#endif // GPCL_ANONYMOUS_SHARED_MEMORY_HPP
