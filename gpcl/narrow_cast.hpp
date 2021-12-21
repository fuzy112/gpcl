//
// narrow_cast.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_NARROW_CAST_HPP
#define GPCL_NARROW_CAST_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/zstring.hpp>

#include <exception>
#include <type_traits>

namespace gpcl {

/// Cast an integer to a narrower type.
/** It is undefined behaviour if the target type cannot hold the value.
 *
 *  @par Example
 * @code {cpp}
 * long a;
 * short b = narrow_cast<short>(a);
 * @endcode
 *
 */
template <typename To, typename From>
constexpr To narrow_cast(From F) noexcept
{
  static_assert(std::is_arithmetic<From>() && std::is_arithmetic<To>(),
                "Both From and To must be arithmetic type");

  To T = static_cast<To>(F);
  GPCL_ASSERT_CONST(static_cast<From>(T) == F);

  return T;
}

class bad_narrow_cast : public std::exception
{
public:
  bad_narrow_cast() = default;

  czstring<> what() const noexcept final { return "bad_narrow_cast"; }
};

/// Cast an integer to a narrower type.
/** @throws bad_narrow_cast if the target type cannot hold the value.
 * @par Example
 * @code {cpp}
 * long a;
 *
 * short b = narrow<short>(a);
 * @endcode
 */
template <typename To, typename From>
To narrow(From F)
{
  static_assert(std::is_arithmetic<From>() && std::is_arithmetic<To>(),
                "Both From and To must be arithmetic type");

  To T = static_cast<To>(F);
  if (static_cast<From>(T) != F)
    GPCL_THROW(bad_narrow_cast());

  return T;
}

} // namespace gpcl

#endif
