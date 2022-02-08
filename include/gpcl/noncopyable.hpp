//
// noncopyable.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_NONCOPYABLE_HPP
#define GPCL_NONCOPYABLE_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

class noncopyable
{
public:
  constexpr noncopyable() noexcept = default;

  noncopyable(const noncopyable &) = delete;
  noncopyable(noncopyable &&) = delete;

  auto operator=(const noncopyable &) -> noncopyable & = delete;
  auto operator=(noncopyable &&) -> noncopyable & = delete;
};

} // namespace gpcl

#endif // GPCL_NONCOPYABLE_HPP
