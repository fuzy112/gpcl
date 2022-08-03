//
// bad_any_cast.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BAD_ANY_CAST_HPP
#define GPCL_BAD_ANY_CAST_HPP

#include <gpcl/detail/config.hpp>

#include <exception>
#include <typeinfo>

namespace gpcl {

class bad_any_cast : public std::bad_cast
{
public:
  bad_any_cast() noexcept = default;

  bad_any_cast(const bad_any_cast &) noexcept = default;

  bad_any_cast &operator=(const bad_any_cast &) noexcept = default;

  const char *what() const noexcept override { return "bad_any_cast"; }
};

} // namespace gpcl

#endif // GPCL_BAD_ANY_CAST_HPP
