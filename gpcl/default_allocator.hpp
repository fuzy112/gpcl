#ifndef GPCL_DEFAULT_ALLOCATOR_HPP
#define GPCL_DEFAULT_ALLOCATOR_HPP

#include <gpcl/detail/config.hpp>

#if defined GPCL_DEBUG
#  include <gpcl/detail/debug_allocator.hpp>
#endif

#include <memory>

namespace gpcl {

#if defined GPCL_DEBUG
template <typename T>
using default_allocator = detail::debug_allocator<T>;

#else
template <typename T>
using default_allocator = std::allocator<T>;
#endif

} // namespace gpcl

#endif // GPCL_DEFAULT_ALLOCATOR_HPP
