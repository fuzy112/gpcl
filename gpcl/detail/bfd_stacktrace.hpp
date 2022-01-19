#ifndef GPCL_DETAIL_BFD_STACKTRACE_HPP
#define GPCL_DETAIL_BFD_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>

#include <string>

namespace gpcl::detail
{

GPCL_DECL std::string bfd_stacktrace_entry_description(const void *address) noexcept;
GPCL_DECL std::string bfd_stacktrace_entry_source_file(const void *address) noexcept;
GPCL_DECL std::uint_least32_t bfd_stacktrace_entry_source_line(const void *address) noexcept;

}

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/bfd_stacktrace.ipp>
#endif

#endif // GPCL_DETAIL_BFD_STACKTRACE_HPP
