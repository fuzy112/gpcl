//
// expected.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_EXPECTED_HPP
#define GPCL_IMPL_EXPECTED_HPP

#include <gpcl/expected.hpp>
#include <gpcl/optional.hpp>

namespace gpcl {

template <typename T, typename E>
optional<T> expected<T, E>::as_optional() const &
{
  if (*this)
    return optional<T>(**this);
  return nullopt;
}

template <typename T, typename E>
optional<T> expected<T, E>::as_optional() &&
{
  if (*this)
    return optional<T>(*detail::move(*this));
  return nullopt;
}

template <typename T, typename E>
optional<E> expected<T, E>::as_optional_error() const &
{
  if (!*this)
    return optional<T>(this->error());
  return nullopt;
}

template <typename T, typename E>
optional<E> expected<T, E>::as_optional_error() &&
{
  if (!*this)
    return optional<T>(detail::move(*this).error());
  return nullopt;
}
template <typename T, typename E>
template <
    typename U, typename G,
    detail::enable_if_t<
        detail::conjunction_v<detail::negate<std::is_convertible<const G &, E>>,
                              std::is_void<T>, std::is_void<U>>,
        int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs)
{
  this->ok_ = bool(rhs);
  if (!this->ok_)
  {
    new (&this->val_) unexpected<E>(rhs.error());
  }
}

template <typename T, typename E>
template <
    typename U,
    detail::enable_if_t<
        detail::conjunction_v<
            detail::negate<std::is_void<U>>,
            detail::negate<std::is_same<expected<T, E>, std::decay_t<U>>>,
            detail::negate<detail::conjunction<
                std::is_scalar<T>, std::is_same<T, std::decay_t<U>>>>,
            std::is_constructible<T, U &&>,
            std::is_assignable<std::add_lvalue_reference_t<T>, U &&>,
            detail::disjunction<std::is_nothrow_constructible<T, U &&>,
                                detail::is_nothrow_move_constructible<E>>>,
        int>>
expected<T, E> &expected<T, E>::operator=(U &&v)
{
  if (*this)
  {
    this->val_ = detail::forward<U>(v);
  }
  else
  {
    if constexpr (detail::is_nothrow_constructible_v<T, U>)
    {
      this->err_.unexpected<E>::~unexpected();
      new (&this->val_) T(detail::forward<U>(v));
      this->ok_ = true;
    }
    else
    {
      unexpected<E> tmp(detail::move(this->error()));
      this->err_.unexpected<E>::~unexpected();
      GPCL_TRY { new (&this->val_) T(detail::forward<U>(v)); }
      GPCL_CATCH(...) { new (&this->err_) unexpected<E>(detail::move(tmp)); }
      GPCL_CATCH_END
      this->ok_ = true;
    }
  }
  return *this;
}

template <typename T, typename E>
template <
    typename G,
    detail::enable_if_t<
        detail::conjunction_v<detail::is_nothrow_move_constructible<G>,
                              detail::is_move_assignable<G>, std::is_void<T>>,
        int>>
expected<T, E> &expected<T, E>::operator=(unexpected<G> &&e)
{
  if (*this)
  {
    new (&this->err_) unexpected<E>(detail::move(e.value()));
    this->ok_ = false;
  }
  else
  {
    this->err_ = detail::move(e);
  }
  return *this;
}

template <typename T, typename E>
template <typename G,
          detail::enable_if_t<
              detail::conjunction_v<detail::is_nothrow_move_constructible<G>,
                                    detail::is_move_assignable<G>,
                                    detail::negate<std::is_void<T>>>,
              int>>
expected<T, E> &expected<T, E>::operator=(unexpected<G> &&e)
{
  if (*this)
  {
    this->val_.T::~T();
    new (&this->err_) unexpected<E>(detail::move(e.value()));
    this->ok_ = false;
  }
  else
  {
    this->err_ = detail::move(e);
  }
  return *this;
}

template <typename T, typename E>
template <
    typename U, typename G,
    detail::enable_if_t<detail::conjunction_v<std::is_convertible<G &&, E>,
                                              std::is_void<T>, std::is_void<U>>,
                        int>>
constexpr expected<T, E>::expected(expected<U, G> &&rhs)
{
  this->ok_ = bool(rhs);
  if (!this->ok_)
  {
    new (&this->err_) unexpected<E>(detail::move(rhs).error());
  }
}

template <typename T, typename E>
template <
    typename U, typename G,
    detail::enable_if_t<
        detail::conjunction_v<
            detail::disjunction<detail::negate<std::is_convertible<U &&, T>>,
                                detail::negate<std::is_convertible<G &&, E>>>,
            detail::expected_convert_constructible<T, E, U, G>>,
        int>>
constexpr expected<T, E>::expected(expected<U, G> &&rhs)
{
  this->ok_ = bool(rhs);
  if (this->ok_)
  {
    new (&this->val_) T(detail::move(*rhs));
  }
  else
  {
    new (&this->err_) unexpected<E>(detail::move(rhs).error());
  }
}
template <typename T, typename E>
template <typename U, typename G,
          detail::enable_if_t<
              detail::conjunction_v<
                  std::is_convertible<U &&, T>, std::is_convertible<G &&, E>,
                  detail::expected_convert_constructible<T, E, U, G>>,
              int>>
constexpr expected<T, E>::expected(expected<U, G> &&rhs)
{
  this->ok_ = bool(rhs);
  if (this->ok_)
  {
    new (&this->val_) T(detail::move(*rhs));
  }
  else
  {
    new (&this->err_) unexpected<E>(detail::move(rhs).error());
  }
}

template <typename T, typename E>
template <
    typename U, typename G,
    detail::enable_if_t<detail::conjunction_v<std::is_convertible<const G &, E>,
                                              std::is_void<T>, std::is_void<U>>,
                        int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs)
{
  this->ok_ = bool(rhs);
  if (!this->ok_)
  {
    new (&this->val_) unexpected<E>(rhs.error());
  }
}

template <typename T, typename E>
template <typename U, typename G,
          detail::enable_if_t<
              detail::conjunction_v<
                  detail::disjunction<
                      detail::negate<std::is_convertible<const U &, T>>,
                      detail::negate<std::is_convertible<const G &, E>>>,
                  detail::expected_convert_constructible<T, E, U, G>>,
              int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs)
{
  this->ok_ = bool(rhs);
  if (this->ok_)
  {
    new (&this->val_) T(*rhs);
  }
  else
  {
    new (&this->err_) unexpected<E>(rhs.error());
  }
}

template <typename T, typename E>
template <
    typename U, typename G,
    detail::enable_if_t<detail::conjunction_v<
                            std::is_convertible<const U &, T>,
                            std::is_convertible<const G &, E>,
                            detail::expected_convert_constructible<T, E, U, G>>,
                        int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs)
{
  this->ok_ = bool(rhs);
  if (this->ok_)
  {
    new (&this->val_) T(*rhs);
  }
  else
  {
    new (&this->err_) unexpected<E>(rhs.error());
  }
}

} // namespace gpcl

#endif