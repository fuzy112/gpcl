//
// identity.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IDENTITY_HPP
#define GPCL_IDENTITY_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

struct identity_t
{
  using is_transparent = identity_t;

  template <typename T>
  constexpr T &&operator()(T &&t) const noexcept
  {
    return static_cast<T &&>(t);
  }
};

inline constexpr identity_t identity{};

} // namespace gpcl

#endif // !GPCL_IDENTITY_HPP
