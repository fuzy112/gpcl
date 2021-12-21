//
// enable_shared_from_this.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ENABLE_SHARED_FROM_THIS_HPP
#define GPCL_ENABLE_SHARED_FROM_THIS_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/weak_ptr.hpp>

namespace gpcl {

namespace detail {
class enable_shared_from_this_base
{
};
} // namespace detail

template <typename Derived>
class enable_shared_from_this : public detail::enable_shared_from_this_base
{
  mutable weak_ptr<Derived> weak_this;

  template <typename T>
  friend class shared_ptr;

protected:
  constexpr enable_shared_from_this() noexcept = default;

  enable_shared_from_this(const enable_shared_from_this &) noexcept {}

  enable_shared_from_this &operator=(const enable_shared_from_this &) noexcept
  {
    return *this;
  }

public:
  shared_ptr<Derived> shared_from_this()
  {
    return shared_ptr<Derived>(weak_this);
  }

  shared_ptr<Derived const> shared_from_this() const
  {
    return shared_ptr<Derived>(weak_this);
  }

  shared_ptr<Derived> weak_from_this() noexcept { return weak_this; }

  shared_ptr<Derived const> weak_from_this() const noexcept
  {
    return weak_this;
  }
};

} // namespace gpcl

#endif // GPCL_ENABLE_SHARED_FROM_THIS_HPP
