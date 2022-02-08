//
// bfd_stacktrace.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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

#endif // GPCL_DETAIL_BFD_STACKTRACE_HPP
