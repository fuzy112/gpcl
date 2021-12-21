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

struct swap_t : swap_detail::swap_impl
{
  using swap_detail::swap_impl::operator();
};

inline constexpr swap_t swap{};

} // namespace gpcl

#endif // GPCL_SWAP_HPP
