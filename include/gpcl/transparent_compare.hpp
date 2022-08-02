#ifndef GPCL_TRANSPARENT_COMPARE_HPP
#define GPCL_TRANSPARENT_COMPARE_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {

template <typename Compare, typename = void>
struct is_transparent_compare : std::false_type
{
};

template <typename Compare>
struct is_transparent_compare<Compare,
                              std::void_t<typename Compare::is_transparent>>
    : std::true_type
{
};

} // namespace gpcl

#endif // GPCL_TRANSPARENT_COMPARE_HPP
