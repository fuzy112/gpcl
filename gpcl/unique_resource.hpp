//
// unique_resource.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_UNIQUE_RESOURCE
#define GPCL_UNIQUE_RESOURCE

#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/optional.hpp>

namespace gpcl {

namespace detail {
template <typename R, typename D>
class unique_resource_base
{
public:
  using stored_resource_handle_type = R;
  using underlying_resource_handle_type = R;
  using deleter_type = D;
};

template <typename R, typename D>
class unique_resource_base<R &, D>
{
public:
  using stored_resource_handle_type = std::reference_wrapper<R>;
  using underlying_resource_handle_type = R &;
  using deleter_type = D;
};
} // namespace detail

template <typename R, typename D>
class unique_resource : public detail::unique_resource_base<R, D>
{
private:
  using base_type = detail::unique_resource_base<R, D>;

  detail::compressed_pair<typename base_type::stored_resource_handle_type,
                          typename base_type::deleter_type>
      p_;

  bool owns_resource_{};

public:
  unique_resource() = default;

  template <typename RR, typename DD>
  unique_resource(RR &&r, DD &&d) noexcept(
      std::is_nothrow_constructible_v<std::decay_t<R>, R>
          &&std::is_nothrow_constructible_v<std::decay_t<D>, D>)
      : p_(std::forward<RR>(r), std::forward<DD>(d)),
        owns_resource_(true)
  {
  }

  unique_resource(unique_resource &&other) noexcept(
      std::is_nothrow_move_constructible_v<R>
          &&std::is_nothrow_move_constructible_v<D>)
      : p_(std::move(other).p_),
        owns_resource_(detail::exchange(other.owns_resource_, false))
  {
  }

  ~unique_resource() noexcept { reset(); }

  // @FIXME: thread safty
  unique_resource &operator=(unique_resource &&other) noexcept(
      std::is_nothrow_move_assignable_v<R>
          &&std::is_nothrow_move_assignable_v<D>)
  {
    reset();
    p_ = std::move(other.p_);
    owns_resource_ = std::exchange(other.owns_resource_, false);
    return *this;
  }

  void swap(unique_resource &other) noexcept(
      std::is_nothrow_swappable_v<R> &&std::is_nothrow_swappable_v<D>)
  {
    using gpcl::swap;
    swap(p_, other.p_);
    swap(owns_resource_, other.owns_resource_);
  }

  /// Releases the ownership of the managed resource if any. The destructor will
  /// not execute the deleter after the call, unless reset is called later for
  /// managing new resource.
  void release() noexcept { owns_resource_ = false; }

  /// Disposes the resource by calling the deleter with the underlying resource
  /// handle if the unique_resource owns it. The unique_resource does not own
  /// the resource after the call.
  void reset() noexcept
  {
    if (owns_resource_)
    {
      p_.second()(p_.first());
      owns_resource_ = false;
    }
  }

  template <typename RR>
  void reset(RR &&r)
  {
    reset();
    p_.second() = std::forward<RR>(r);
  }

  /// Accesses the underlying resource handle.
  const R &get() const noexcept { return p_.first(); }

  /// Accesses the deleter object which would be used for disposing the managed
  /// resource.
  const D &get_deleter() const noexcept { return p_.second(); }

  /// Access the object or function pointed by the underlying resource handle
  /// which is a pointer. This function participates in overload resolution only
  /// if std::is_pointer_v<R> is true and
  /// std::is_void_v<std::remove_pointer_t<R>> is false. If the resource handle
  /// is not pointing to an object or a function, the behavior is undefined.
  std::add_lvalue_reference_t<std::remove_pointer_t<R>>
  operator*() const noexcept
  {
    return *p_.first();
  }

  /// Get a copy of the underlying resource handle which is a pointer. This
  /// function participates in overload resolution only if std::is_pointer_v<R>
  /// is true. The return value is typically used to access the pointed object.
  R operator->() const noexcept { return *p_.first(); }

  explicit operator bool() const noexcept { return owns_resource_; }
};

/// Creates a unique_resource, initializes its stored resource handle is
/// initialized with std::forward<R>(r) and its deleter with std::forward<D>(d).
/// The created unique_resource owns the resource if and only if bool(r ==
/// invalid) is false.
template <class R, class D, class S = std::decay_t<R>>
unique_resource<std::decay_t<R>, std::decay_t<D>> make_unique_resource_checked(
    R &&r, const S &invalid,
    D &&d) noexcept(std::is_nothrow_constructible_v<std::decay_t<R>, R>
                        &&std::is_nothrow_constructible_v<std::decay_t<D>, D>)
{
  if (r == invalid)
    return unique_resource<std::decay_t<R>, std::decay_t<D>>();
  else
    return unique_resource<std::decay_t<R>, std::decay_t<D>>(
        std::forward<R>(r), std::forward<D>(d));
}

namespace swap_detail {
template <typename R, typename D>
void swap(unique_resource<R, D> &x,
          unique_resource<R, D> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}
} // namespace swap_detail

} // namespace gpcl

#endif
