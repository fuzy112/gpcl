//
// tag_invoke.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_TAG_INVOKE_HPP
#define GPCL_TAG_INVOKE_HPP

#include <gpcl/detail/tag_invoke.hpp>

#include <type_traits>

namespace gpcl {

/// The meta-customisation-point-object.
/// Other customisation points can be implemented in terms of `tag_invoke`.
///
/// @ingroup customisation_point
inline constexpr detail::tag_invoke_impl tag_invoke{};

template <typename Tag, typename... Args>
using tag_invoke_result_t =
    decltype(gpcl::tag_invoke(std::declval<Tag>(), std::declval<Args>()...));

} // namespace gpcl

#endif // GPCL_TAG_INVOKE_HPP
