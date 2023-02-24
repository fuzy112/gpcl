//
// assertion_failure.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2023 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ASSERTION_FAILURE_HPP
#define GPCL_DETAIL_ASSERTION_FAILURE_HPP

#include <gpcl/detail/config.hpp>

#include <cstdint>

namespace gpcl {
namespace detail {

GPCL_NORETURN GPCL_DECL void assertion_failure(const char *expr,
                                               const char *file,
                                               std::uint_least32_t line,
                                               const char *func);

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_ASSERTION_FAILURE_HPP
