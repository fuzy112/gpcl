//
// assertion_failure.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ASSERTION_FAILURE_HPP
#define GPCL_DETAIL_ASSERTION_FAILURE_HPP

#include <gpcl/detail/config.hpp>

#include <cstdint>

namespace gpcl::detail {

GPCL_DECL void assertion_failure(const char *expr, const char *file,
                                 std::uint_least32_t line, const char *func);

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ASSERTION_FAILURE_HPP
