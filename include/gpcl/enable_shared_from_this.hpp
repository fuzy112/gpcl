//
// enable_shared_from_this.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
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

/// A mixin that enables obtaining a shared reference from `this`.
/**
 * @tparam Derived The derived class.
 * 
 * @par Example
 * This is to be used as public base class.
 * 
 * @code {.cpp}
 * class my_class : public enable_shared_from_this<my_class>
 * {
 * public:
 *    shared_ptr<my_class> get_shared_reference()
 *    {
 *      return shared_from_this();
 *    }
 * };
 * 
 * auto my_object = make_shared<my_class>();
 * auto sp = my_object->get_shared_reference();
 * @endcode
 * 
 * @sa shared_ptr
 */
template <typename Derived>
class enable_shared_from_this : public detail::enable_shared_from_this_base
{
  /// A weak reference to the object itself.
  mutable weak_ptr<Derived> weak_this;

  template <typename T>
  friend class gpcl::shared_ptr;

protected:
  constexpr enable_shared_from_this() noexcept = default;

  /// Copy constructor.
  /**
   * @note weak_this is not copied.
   */
  enable_shared_from_this(const enable_shared_from_this &) noexcept {}

  /// Copy assignment.
  /**
   * @note weak_this is not copied.
   */
  enable_shared_from_this &operator=(const enable_shared_from_this &) noexcept
  {
    return *this;
  }

public:
  /// Obtains a shared reference from `this`.
  /**
   * @throws bad_weak_ptr If `this` is not managed by shared_ptr.
   */
  shared_ptr<Derived> shared_from_this()
  {
    return shared_ptr<Derived>(weak_this);
  }

  /// Obtains a shared reference from `this`.
  /**
   * @throws bad_weak_ptr If `this` is not managed by shared_ptr.
   */
  shared_ptr<Derived const> shared_from_this() const
  {
    return shared_ptr<Derived>(weak_this);
  }

  /// Obtains a weak reference from `this`.
  shared_ptr<Derived> weak_from_this() noexcept { return weak_this; }

  /// Obtains a weak reference from `this`.
  shared_ptr<Derived const> weak_from_this() const noexcept
  {
    return weak_this;
  }
};

} // namespace gpcl

#endif // GPCL_ENABLE_SHARED_FROM_THIS_HPP
