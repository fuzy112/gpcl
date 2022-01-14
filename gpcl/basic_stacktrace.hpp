#ifndef GPCL_BASIC_STACKTRACE_HPP
#define GPCL_BASIC_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>

#if defined(GPCL_WINDOWS)
#  include <gpcl/detail/win_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#elif defined(GPCL_LINUX)
#  include <gpcl/detail/linux_stacktrace.hpp>
#  define GPCL_STACKTRACE 1
#endif

#include <memory>

namespace gpcl {
#if defined(GPCL_WINDOWS)
template <typename Allocator>
using basic_stacktrace = detail::basic_win_stacktrace<Allocator>;
using stacktrace_entry = detail::win_stacktrace_entry;
#elif defined(GPCL_LINUX)
template <typename Allocator>
using basic_stacktrace = detail::basic_linux_stacktrace<Allocator>;
using stacktrace_entry = detail::linux_stacktrace_entry;
#endif

#ifdef GPCL_STACKTRACE
using stacktrace = basic_stacktrace<std::allocator<stacktrace_entry>>;
#endif

} // namespace gpcl

#endif // GPCL_BASIC_STACKTRACE_HPP
