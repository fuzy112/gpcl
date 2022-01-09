#ifndef GPCL_STATIC_CONST_HPP
#define GPCL_STATIC_CONST_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

/// Variable template for a static const object of type T.
/// Used to make sure the variable has external linkage.
/// @sa http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html,
template <typename T>
constexpr T static_const{};

} // namespace gpcl

#endif // GPCL_STATIC_CONST_HPP
