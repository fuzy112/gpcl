//
// swap.hpp
// ~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SWAP_HPP
#define GPCL_SWAP_HPP

#include <gpcl/detail/config.hpp>

#include <utility>

namespace gpcl {
namespace swap_detail {
using std::swap;

struct swap_impl
{
  template <typename T1, typename T2>
  constexpr void operator()(T1 &x1, T2 &x2) const
      noexcept(noexcept(swap(x1, x2)))
  {
    swap(x1, x2);
  }
};
} // namespace swap_detail

/// Swap two objects.
/**
 * @ingroup customization_point
 *
 * @par Example
 * @code{.cpp}
 * using gpcl::swap;
 *
 * swap(x, y);
 * @endcode
 *
 * or:
 * @code{.cpp}
 * gpcl::swap(x, y);
 * @endcode
 */
#ifdef GPCL_DOXYGEN
template <typename T>
void swap(T &x, T &y);
#else
inline constexpr swap_detail::swap_impl swap{};
#endif

} // namespace gpcl

#endif // GPCL_SWAP_HPP
