//
// unreachable.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_UNREACHABLE_HPP
#define GPCL_DETAIL_UNREACHABLE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/source_location.hpp>

#include <gpcl/zstring.hpp>

namespace gpcl {
namespace detail {

GPCL_NORETURN GPCL_DECL void
unreachable_internal(gpcl::czstring<> message,
                     source_location location) GPCL_NOEXCEPT;

}
} // namespace gpcl

#endif // GPCL_DETAIL_UNREACHABLE_HPP
