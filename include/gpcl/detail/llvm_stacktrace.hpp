//
// llvm_stacktrace.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_LLVM_STACKTRACE_HPP
#define GPCL_DETAIL_LLVM_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/optional_fwd.hpp>

#include <string>

namespace gpcl::detail {

struct llvm_line_info;
GPCL_DECL optional<llvm_line_info> llvm_symbolize_code(const void *address);

GPCL_DECL std::string llvm_stacktrace_entry_description(const void *address);
GPCL_DECL std::string llvm_stacktrace_entry_source_file(const void *address);
GPCL_DECL std::uint_least32_t
llvm_stacktrace_entry_source_line(const void *address);

} // namespace gpcl::detail

#endif // GPCL_DETAIL_LLVM_STACKTRACE_HPP
