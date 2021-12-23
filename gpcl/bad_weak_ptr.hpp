//
// bad_weak_ptr.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BAD_WEAK_PTR_HPP
#define GPCL_BAD_WEAK_PTR_HPP

#include <gpcl/detail/config.hpp>

#include <exception>

namespace gpcl {

/// @see gpcl::weak_ptr
class bad_weak_ptr : public std::exception
{
public:
  bad_weak_ptr() noexcept = default;

  /// Copy constructor.
  bad_weak_ptr(const bad_weak_ptr &) noexcept = default;

  /// @returns "bad_weak_ptr".
  const char *what() const noexcept override { return "bad_weak_ptr"; }
};

} // namespace gpcl

#endif // GPCL_BAD_WEAK_PTR_HPP
