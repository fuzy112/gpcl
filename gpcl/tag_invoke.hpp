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
