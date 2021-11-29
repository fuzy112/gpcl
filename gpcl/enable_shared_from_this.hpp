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

template <typename Derived>
class enable_shared_from_this
{
  mutable weak_ptr<Derived> weak_this;

  template <typename, typename>
  friend struct detail::shared_ptr_hooks;

  void init_weak(shared_ptr<Derived> const &sp) noexcept
  {
    GPCL_ASSERT(this == sp.get());
    weak_this = sp;
  }

public:
  using enable_shared_from_this_inherited = int;

  constexpr enable_shared_from_this() noexcept = default;

  enable_shared_from_this(const enable_shared_from_this &) noexcept {}

  enable_shared_from_this &operator=(const enable_shared_from_this &) noexcept
  {
    return *this;
  }

  shared_ptr<Derived> shared_from_this() { return shared_ptr<Derived>(weak_this); }

  shared_ptr<Derived const> shared_from_this() const
  {
    return shared_ptr<Derived>(weak_this);
  }

  shared_ptr<Derived> weak_from_this() { return weak_this; }

  shared_ptr<Derived const> weak_from_this() const { return weak_this; }
};

namespace detail {
template <typename T>
struct shared_ptr_hooks<T, detail::void_t<typename T::enable_shared_from_this_inherited>>
{
  template <typename Y>
  static void on_creation(Y *p, const shared_ptr<T> &sp) noexcept
  {
    if (sp.get() == p)
      sp->init_weak(sp);
  }
};
} // namespace detail

} // namespace gpcl

#endif // GPCL_ENABLE_SHARED_FROM_THIS_HPP
