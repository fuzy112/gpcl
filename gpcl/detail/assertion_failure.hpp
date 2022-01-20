#ifndef GPCL_DETAIL_ASSERTION_FAILURE_HPP
#define GPCL_DETAIL_ASSERTION_FAILURE_HPP

#include <gpcl/detail/config.hpp>

#include <cstdint>
#include <cstdlib>

namespace gpcl::detail {

struct assertion_failure_hook_tag
{
};

inline void assertion_failure(...) {}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ASSERTION_FAILURE_HPP
