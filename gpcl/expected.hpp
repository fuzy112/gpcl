//
// expected.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EXPECTED_HPP
#define GPCL_EXPECTED_HPP

#include <gpcl/bad_expected_access.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/expected.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/expected_fwd.hpp>
#include <gpcl/in_place.hpp>
#include <gpcl/optional_fwd.hpp>
#include <gpcl/unexpected.hpp>

#include <functional>

namespace gpcl {

enum use_expected_t : bool
{
  use_expected
};

/// Represents either a value T or an error E.
///
/// expected<T, E> is a pointer-like class.
/// Implicit conversion to bool returns true if there is a usable value. The
/// unary * and -> operators provide pointer like access to the value. Accessing
/// the value when there is an error has undefined behavior.
///
template <typename T, typename E>
class [[nodiscard]] expected : public detail::expected_move_assign_base<T, E>
{
  using base_type = detail::expected_move_assign_base<T, E>;

public:
  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<error_type>;

  template <typename U>
  using rebind = expected<U, error_type>;
  template <typename G>
  using rebind_error = expected<T, G>; // extension

  constexpr expected() = default;
  constexpr expected(const expected &rhs) = default;
  constexpr expected(expected && rhs) = default;

  template <
      typename... Args,
      detail::enable_if_t<detail::is_constructible_v<T, Args &&...>, int> = 0>
  inline constexpr explicit expected(in_place_t, Args && ... args)
      : base_type(in_place, detail::forward<Args>(args)...)
  {
  }

  template <typename U, typename... Args,
            detail::enable_if_t<detail::is_constructible_v<
                                    T, std::initializer_list<U> &, Args &&...>,
                                int> = 0>
  inline constexpr expected(in_place_t, std::initializer_list<U> ilist,
                            Args && ... args)
      : base_type(in_place, ilist, detail::forward<Args>(args)...)
  {
  }

  template <typename U, typename G,
            detail::enable_if_t<
                detail::conjunction_v<
                    std::is_convertible<const U &, T>,
                    std::is_convertible<const G &, E>,
                    detail::expected_convert_constructible<T, E, U, G>>,
                int>
                Dummy1 = 0>
  inline constexpr expected(const expected<U, G> &rhs);

  template <typename U, typename G,
            detail::enable_if_t<
                detail::conjunction_v<
                    detail::disjunction<
                        detail::negate<std::is_convertible<const U &, T>>,
                        detail::negate<std::is_convertible<const G &, E>>>,
                    detail::expected_convert_constructible<T, E, U, G>>,
                int>
                Dummy2 = 0>
  inline explicit constexpr expected(const expected<U, G> &rhs);

  template <typename U, typename G,
            detail::enable_if_t<
                detail::conjunction_v<std::is_convertible<const G &, E>,
                                      std::is_void<T>, std::is_void<U>>,
                int>
                Dummy3 = 0>
  inline constexpr expected(const expected<U, G> &rhs);

  template <
      typename U, typename G,
      detail::enable_if_t<detail::conjunction_v<
                              detail::negate<std::is_convertible<const G &, E>>,
                              std::is_void<T>, std::is_void<U>>,
                          int>
          Dummy4 = 0>
  inline explicit constexpr expected(const expected<U, G> &rhs);

  template <typename U, typename G,
            detail::enable_if_t<
                detail::conjunction_v<
                    std::is_convertible<U &&, T>, std::is_convertible<G &&, E>,
                    detail::expected_convert_constructible<T, E, U, G>>,
                int>
                Dummy1 = 0>
  inline constexpr expected(expected<U, G> && rhs);

  template <
      typename U, typename G,
      detail::enable_if_t<
          detail::conjunction_v<
              detail::disjunction<detail::negate<std::is_convertible<U &&, T>>,
                                  detail::negate<std::is_convertible<G &&, E>>>,
              detail::expected_convert_constructible<T, E, U, G>>,
          int>
          Dummy2 = 0>
  inline explicit constexpr expected(expected<U, G> && rhs);

  template <typename U, typename G,
            detail::enable_if_t<
                detail::conjunction_v<std::is_convertible<G &&, E>,
                                      std::is_void<T>, std::is_void<U>>,
                int>
                Dummy3 = 0>
  inline constexpr expected(expected<U, G> && rhs);

  template <
      typename U, typename G,
      detail::enable_if_t<
          detail::conjunction_v<detail::negate<std::is_convertible<G &&, E>>,
                                std::is_void<T>, std::is_void<U>>,
          int>
          Dummy4 = 0>
  inline explicit constexpr expected(expected<U, G> && rhs);

  template <
      class U = T,
      detail::enable_if_t<detail::is_convertible_v<U &&, T>, int> = 0,
      detail::enable_if_t<
          detail::conjunction_v<
              detail::negate<std::is_void<T>>, std::is_constructible<T, U &&>,
              detail::negate<std::is_same<std::decay_t<U>, in_place_t>>,
              detail::negate<std::is_same<std::decay_t<U>, expected>>,
              detail::negate<std::is_same<std::decay_t<U>, unexpected<E>>>>,
          int>
          Dummy1 = 0>
  inline constexpr expected(U && v) : base_type(in_place, detail::forward<U>(v))
  {
  }

  template <
      class U = T,
      detail::enable_if_t<detail::negate_v<std::is_convertible<U &&, T>>, int> =
          0,
      detail::enable_if_t<
          detail::conjunction_v<
              detail::negate<std::is_void<T>>, std::is_constructible<T, U &&>,
              detail::negate<std::is_same<std::decay_t<U>, in_place_t>>,
              detail::negate<std::is_same<std::decay_t<U>, expected>>,
              detail::negate<std::is_same<std::decay_t<U>, unexpected<E>>>>,
          int>
          Dummy2 = 0>
  inline explicit constexpr expected(U && v)
      : base_type(in_place, detail::forward<U>(v))
  {
  }

  template <typename G = E,
            detail::enable_if_t<
                detail::conjunction_v<std::is_convertible<const G &, E>,
                                      std::is_constructible<E, const G &>>,
                int>
                Dummy1 = 0>
  inline constexpr expected(const unexpected<G> &e) : base_type(unexpect, e)
  {
  }

  template <
      typename G = E,
      detail::enable_if_t<detail::conjunction_v<
                              detail::negate<std::is_convertible<const G &, E>>,
                              std::is_constructible<E, const G &>>,
                          int>
          Dummy2 = 0>
  inline explicit constexpr expected(const unexpected<G> &e)
      : base_type(unexpect, e)
  {
  }

  template <
      typename G = E,
      detail::enable_if_t<detail::conjunction_v<std::is_convertible<G &&, E>,
                                                std::is_constructible<E, G &&>>,
                          int>
          Dummy1 = 0>
  inline constexpr expected(unexpected<G> && e) : base_type(unexpect, e)
  {
  }

  template <
      typename G = E,
      detail::enable_if_t<
          detail::conjunction_v<detail::negate<std::is_convertible<G &&, E>>,
                                std::is_constructible<E, G &&>>,
          int>
          Dummy2 = 0>
  inline explicit constexpr expected(unexpected<G> && e)
      : base_type(unexpect, e)
  {
  }

  template <typename... Args,
            detail::enable_if_t<detail::is_constructible_v<E, Args...>, int> =
                0>
  inline constexpr explicit expected(unexpect_t, Args && ... args)
      : base_type(unexpect, detail::forward<Args>(args)...)
  {
  }

  template <typename U, typename... Args,
            detail::enable_if_t<detail::is_constructible_v<
                                    E, std::initializer_list<U> &, Args...>,
                                int> = 0>
  inline constexpr explicit expected(unexpect_t, std::initializer_list<U> & il,
                                     Args && ... args)
      : base_type(unexpect, il, detail::forward<Args>(args)...)
  {
  }

  expected &operator=(const expected &rhs) = default;
  expected &operator=(expected &&rhs) = default;

  template <
      typename U = T,
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
          int> = 0>
  inline expected<T, E> &operator=(U &&v);

  template <typename G = E,
            typename std::enable_if<
                detail::conjunction_v<detail::is_nothrow_copy_constructible<G>,
                                      detail::is_copy_assignable<G>,
                                      detail::negate<std::is_void<T>>>,
                int>::type Dummy1 = 0>
  inline expected<T, E> &operator=(const unexpected<G> &e)
  {
    if (*this)
    {
      this->val_.T::~T();
      new (&this->err_) unexpected<E>(e.value());
      this->ok_ = false;
    }
    else
    {
      this->err_ = e;
    }
    return *this;
  }

  template <
      typename G = E,
      typename std::enable_if<
          detail::conjunction_v<detail::is_nothrow_copy_constructible<G>,
                                detail::is_copy_assignable<G>, std::is_void<T>>,
          int>::type Dummy2 = 0>
  inline expected<T, E> &operator=(const unexpected<G> &e)
  {
    if (*this)
    {
      new (&this->err_) unexpected<E>(e.value());
      this->ok_ = false;
    }
    else
    {
      this->err_ = e;
    }
    return *this;
  }

  template <typename G = E,
            detail::enable_if_t<
                detail::conjunction_v<detail::is_nothrow_move_constructible<G>,
                                      detail::is_move_assignable<G>,
                                      detail::negate<std::is_void<T>>>,
                int>
                Dummy1 = 0>
  inline expected<T, E> &operator=(unexpected<G> &&e);

  template <
      typename G = E,
      detail::enable_if_t<
          detail::conjunction_v<detail::is_nothrow_move_constructible<G>,
                                detail::is_move_assignable<G>, std::is_void<T>>,
          int>
          Dummy2 = 0>
  inline expected<T, E> &operator=(unexpected<G> &&e);

  void swap(expected<T, E> & other) noexcept(
      std::is_nothrow_copy_assignable<expected<T, E>>::value)
  {
    auto tmp = detail::move(other);
    other = detail::move(*this);
    *this = detail::move(tmp);
  }

  /// Returns x if bool(*this) is true, otherwise returns
  /// unexpected(error())
  template <typename U>
  inline rebind<U> and_(const rebind<U> &x) const &
  {
    if (*this)
    {
      return x;
    }
    else
    {
      return unexpected<E>(this->error());
    }
  }

  /// Returns x if bool(*this) is true, otherwise returns
  /// unexpected(error())
  template <typename U>
  inline rebind<U> and_(rebind<U> && x) const &
  {
    if (*this)
    {
      return detail::move(x);
    }
    else
    {
      return unexpected<E>(this->error());
    }
  }

  /// Returns x if bool(*this) is true, otherwise returns
  /// unexpected(error())
  template <typename U>
  inline rebind<U> and_(const rebind<U> &x) &&
  {
    if (*this)
    {
      return x;
    }
    else
    {
      return unexpected<E>(detail::move(*this).error());
    }
  }

  /// Returns x if bool(*this) is true, otherwise returns
  /// unexpected(error())
  template <typename U>
  inline rebind<U> and_(rebind<U> && x) &&
  {
    if (*this)
    {
      return detail::move(x);
    }
    else
    {
      return unexpected<E>(detail::move(*this).error());
    }
  }

  /// Maps an expected<T, E> to expected<U, E> by applying a function to the
  /// contained value, leaving the error untouched.
  template <typename F,
            typename U =
                std::invoke_result_t<F, std::add_lvalue_reference_t<const T>>,
            detail::enable_if_t<!detail::is_void_v<U>, int> Dummy1 = 0>
  inline rebind<U> map(F && f) const &
  {
    if (*this)
    {
      return rebind<U>(std::invoke(detail::forward<F>(f), **this));
    }
    else
    {
      return unexpected<E>((*this).error());
    }
  }

  /// Maps an expected<T, E> to expected<U, E> by applying a function to the
  /// contained value, leaving the error untouched.
  template <typename F,
            typename U =
                std::invoke_result_t<F, std::add_lvalue_reference_t<const T>>,
            detail::enable_if_t<detail::is_void_v<U>, int> Dummy2 = 0>
  inline rebind<U> map(F && f) const &
  {
    if (*this)
    {
      std::invoke(detail::forward<F>(f), **this);
      return rebind<U>();
    }
    else
    {
      return unexpected<E>((*this).error());
    }
  }

  /// Maps an expected<T, E> to expected<U, E> by applying a function to the
  /// contained value, leaving the error untouched.
  template <typename F,
            typename U =
                std::invoke_result_t<F, std::add_rvalue_reference_t<T>>,
            detail::enable_if_t<!detail::is_void_v<U>, int> Dummy1 = 0>
  inline rebind<U> map(F && f) &&
  {
    if (*this)
    {
      return rebind<U>(
          std::invoke(detail::forward<F>(f), *detail::move(*this)));
    }
    else
    {
      return unexpected<E>(detail::move(*this).error());
    }
  }

  /// Maps an expected<T, E> to expected<U, E> by applying a function to the
  /// contained value, leaving the error untouched.
  template <typename F,
            typename U =
                std::invoke_result_t<F, std::add_rvalue_reference_t<T>>,
            detail::enable_if_t<detail::is_void_v<U>, int> Dummy2 = 0>
  inline rebind<U> map(F && f) &&
  {
    if (*this)
    {
      std::invoke(detail::forward<F>(f), *detail::move(*this));
      return rebind<U>();
    }
    else
    {
      return unexpected<E>(detail::move(*this).error());
    }
  }

  /// Calls f if the bool(*this) is true, otherwise returns
  /// unexpected(error()).
  template <
      typename F,
      typename Result =
          std::invoke_result_t<F, std::add_lvalue_reference_t<const T>>,
      detail::enable_if_t<detail::is_same_v<typename Result::error_type, E>
                          // && detail::is_expected_v<std::decay_t<Result>>
                          ,
                          int> = 0>
  inline Result and_then(F && f) const &
  {
    if (*this)
    {
      return std::invoke(detail::forward<F>(f), **this);
    }
    else
    {
      return unexpected<E>((*this).error());
    }
  }

  /// Calls f if the bool(*this) is true, otherwise returns
  /// unexpected(error()).
  template <
      typename F,
      typename Result = std::invoke_result_t<F, std::add_rvalue_reference_t<T>>,
      detail::enable_if_t<detail::is_same_v<typename Result::error_type, E>
                          // && detail::is_expected_v<std::decay_t<Result>>
                          ,
                          int> = 0>
  inline Result and_then(F && f) &&
  {
    if (*this)
    {
      return std::invoke(detail::forward<F>(f), *detail::move(*this));
    }
    else
    {
      return unexpected<E>(detail::move(*this).error());
    }
  }

  /// Calls f if bool(*this) is true, otherwise returns unexpected(error()).
  template <typename F, typename V = T,
            typename Result = std::invoke_result_t<F>,
            detail::enable_if_t<detail::is_void_v<V>, int> = 0>
  inline Result and_then(F && f) const
  {
    if (*this)
    {
      return std::invoke(detail::forward<F>(f));
    }
    else
    {
      return unexpected<E>(detail::move(*this).error());
    }
  }

  /// Returns x if bool(*this) is false, otherwise returns expected(*this).
  template <typename G>
  inline rebind_error<G> or_(const rebind_error<G> &x) const &
  {
    if (*this)
    {
      return rebind_error<G>(*this);
    }
    else
    {
      return x;
    }
  }

  /// Returns x if bool(*this) is false, otherwise returns
  /// expected(**this).
  template <typename G>
  inline rebind_error<G> or_(rebind_error<G> && x) const &
  {
    if (*this)
    {
      return rebind_error<G>((*this));
    }
    else
    {
      return detail::move(x);
    }
  }

  /// Returns x if bool(*this) is false, otherwise returns
  /// expected(**this).
  template <typename G>
  inline rebind_error<G> or_(const rebind_error<G> &x) &&
  {
    if (*this)
    {
      return rebind_error<G>(detail::move(*this));
    }
    else
    {
      return x;
    }
  }

  /// Returns x if bool(*this) is false, otherwise returns expected(*this).
  template <typename G>
  inline rebind_error<G> or_(rebind_error<G> && x) &&
  {
    if (*this)
    {
      return rebind_error<G>(detail::move(*this));
    }
    else
    {
      return detail::move(x);
    }
  }

  /// Maps an expected<T, E> to expected<T, G> by applying a function to the
  /// error, leaving the value untouched.
  template <typename F,
            typename G =
                std::invoke_result_t<F, std::add_lvalue_reference_t<const E>>>
  inline rebind_error<G> map_error(F && f) const &
  {
    if (*this)
    {
      return rebind_error<G>(*this);
    }
    else
    {
      return unexpected<E>(std::invoke(detail::forward<F>(f), (*this).error()));
    }
  }

  /// Maps an expected<T, E> to expected<T, G> by applying a function to the
  /// error, leaving the value untouched
  template <typename F,
            typename G =
                std::invoke_result_t<F, std::add_rvalue_reference_t<E>>>
  inline rebind_error<G> map_error(F && f) &&
  {
    if (*this)
    {
      return rebind_error<G>(detail::move(*this));
    }
    else
    {
      return unexpected<E>(
          std::invoke(detail::forward<F>(f), detail::move(*this).error()));
    }
  }

  /// Calls f if bool(*this) is false, otherwise returns expected(**this).
  template <
      typename F,
      typename Result =
          std::invoke_result_t<F, std::add_lvalue_reference_t<const E>>,
      detail::enable_if_t<
          detail::conjunction_v<detail::negate<std::is_void<T>>,
                                std::is_same<typename Result::value_type, T>>,
          int>
          Dummy1 = 0>
  inline Result or_else(F && f) const &
  {
    if (*this)
    {
      return Result(**this);
    }
    else
    {
      return std::invoke(detail::forward<F>(f), (*this).error());
    }
  }

  /// Calls f if bool(*this) is false, otherwise returns expected(**this).
  template <
      typename F,
      typename Result = std::invoke_result_t<F, std::add_rvalue_reference_t<E>>,
      detail::enable_if_t<
          detail::conjunction_v<std::is_same<typename Result::value_type, T>,
                                detail::negate<std::is_void<T>>>,
          int>
          Dummy1 = 0>
  inline Result or_else(F && f) &&
  {
    if (*this)
    {
      return Result(*detail::move(*this));
    }
    else
    {
      return std::invoke(detail::forward<F>(f), detail::move(*this).error());
    }
  }

  /// Calls f if bool(*this) is false, otherwise returns expected(**this).
  template <
      typename F,
      typename Result =
          std::invoke_result_t<F, std::add_lvalue_reference_t<const E>>,
      detail::enable_if_t<
          detail::conjunction_v<std::is_same<typename Result::value_type, T>,
                                std::is_void<T>>,
          int>
          Dummy2 = 0>
  inline Result or_else(F && f) const &
  {
    if (*this)
    {
      return Result();
    }
    else
    {
      return std::invoke(detail::forward<F>(f), (*this).error());
    }
  }

  /// Calls f if bool(*this) is false, otherwise returns expected(**this).
  template <
      typename F,
      typename Result = std::invoke_result_t<F, std::add_rvalue_reference_t<E>>,
      detail::enable_if_t<
          detail::conjunction_v<std::is_same<typename Result::value_type, T>,
                                std::is_void<T>>,
          int>
          Dummy2 = 0>
  inline Result or_else(F && f) &&
  {
    if (*this)
    {
      return Result();
    }
    else
    {
      return std::invoke(detail::forward<F>(f), detail::move(*this).error());
    }
  }

  /// Converts from expected<T, E> to optional<T>.
  inline optional<T> as_optional() const &;

  /// Converts from expected<T, E> to optional<T>.
  inline optional<T> as_optional() &&;

  /// Converts from expected<T, E> to optional<E>.
  inline optional<E> as_optional_error() const &;

  /// Converts from expected<T, E> to optional<E>.
  inline optional<E> as_optional_error() &&;

  /// Applies a function to the contained value if bool(*this) is true,
  /// otherwise returns @c default_.
  template <typename U, typename F,
            typename R =
                std::invoke_result_t<F, std::add_lvalue_reference_t<const T>>>
  inline U map_or(U default_, F && f) const &
  {
    return map(detail::forward<F>(f)).value_or(default_);
  }

  /// Applies a function to the contained value if bool(*this) is true,
  /// otherwise returns default_.
  template <typename U, typename F,
            typename R =
                std::invoke_result_t<F, std::add_rvalue_reference_t<T>>>
  inline U map_or(U default_, F && f) &&
  {
    return detail::move(*this).map(detail::forward<F>(f)).value_or(default_);
  }

  /// Maps an expected<T, E> to U by applying a function to a contained
  /// value, or a fallback function to a contained error.
  template <typename D, typename F,
            typename R1 =
                std::invoke_result_t<D, std::add_lvalue_reference_t<const E>>,
            typename R2 =
                std::invoke_result_t<F, std::add_lvalue_reference_t<const T>>,
            detail::enable_if_t<detail::is_same_v<R1, R2>, int> = 0>
  inline R1 map_or_else(D && d, F && f) const &
  {
    if (*this)
    {
      return std::invoke(detail::forward<F>(f), **this);
    }
    else
    {
      return std::invoke(detail::forward<D>(d), this->error());
    }
  }

  /// Maps an expected<T, E> to U by applying a function to a contained
  /// value, or a fallback function to a contained error.
  template <
      typename D, typename F,
      typename R1 = std::invoke_result_t<D, std::add_rvalue_reference_t<E>>,
      typename R2 = std::invoke_result_t<F, std::add_rvalue_reference_t<T>>,
      detail::enable_if_t<detail::is_same_v<R1, R2>, int> = 0>
  inline R1 map_or_else(D && d, F && f) &&
  {
    if (*this)
    {
      return std::invoke(detail::forward<F>(f), *detail::move(*this));
    }
    else
    {
      return std::invoke(detail::forward<D>(d), detail::move(*this).error());
    }
  }

  /// Convert expected<expected<T, E>, E> to expected<T, E>
  template <typename U = T,
            detail::enable_if_t<
                detail::is_same_v<typename U::error_type, error_type>, int> = 0>
  inline value_type flatten() const &
  {
    return and_then([](const T &x) { return x; });
  }

  /// Convert expected<expected<T, E>, E> to expected<T, E>
  template <typename U = T,
            detail::enable_if_t<
                detail::is_same_v<typename U::error_type, error_type>, int> = 0>
  inline value_type flatten() &&
  {
    return detail::move(*this).and_then([](T &&x) { return detail::move(x); });
  }

  /// Take the ownership of the contained value.
  template <typename U = T,
            detail::enable_if_t<detail::is_move_constructible_v<U>, int> = 0>
  inline value_type take_value()
  {
    return detail::move(*this).value();
  }

  /// Take the ownership of the contained error.
  template <typename U = E,
            detail::enable_if_t<detail::is_move_constructible_v<U>, int> = 0>
  inline error_type take_error()
  {
    return detail::move(*this).error();
  }
};

namespace swap_detail {
template <typename T, typename E>
void swap(expected<T, E> &x, expected<T, E> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}
} // namespace swap_detail

template <typename T, typename E>
constexpr bool operator==(const expected<T, E> &x, const expected<T, E> &y)
{
  if (x && y)
  {
    return *x == *y;
  }

  if (!x && !y)
  {
    return x.error() == y.error();
  }

  return false;
}

template <typename T, typename E>
constexpr bool operator!=(const expected<T, E> &x, const expected<T, E> &y)
{
  return !(x == y);
}

template <typename T, typename E>
constexpr bool operator==(const expected<T, E> &x, const T &y)
{
  if (!x)
    return false;
  return *x == y;
}

template <typename T, typename E>
constexpr bool operator==(const T &x, const expected<T, E> &y)
{
  if (!y)
    return false;
  return x == *y;
}

template <typename T, typename E>
constexpr bool operator!=(const expected<T, E> &x, const T &y)
{
  return !(x == y);
}

template <typename T, typename E>
constexpr bool operator!=(const T &x, const expected<T, E> &y)
{
  return !(x == y);
}

template <typename T, typename E>
constexpr bool operator==(const expected<T, E> &x, const unexpected<E> &y)
{
  if (x)
    return false;
  return x.error() == y.value();
}

template <typename T, typename E>
constexpr bool operator==(const unexpected<E> &x, const expected<T, E> &y)
{
  if (y)
    return false;
  return x.value() == y.error();
}

template <typename T, typename E>
constexpr bool operator!=(const expected<T, E> &x, const unexpected<E> &y)
{
  return !(x == y);
}

template <typename T, typename E>
constexpr bool operator!=(const unexpected<E> &x, const expected<T, E> &y)
{
  return !(x == y);
}

#define GPCL_EXPECTED_TRY(var, exp)                                            \
  auto _gpcl_expected_##var = exp;                                             \
  if (!_gpcl_expected_##var)                                                   \
  {                                                                            \
    return ::gpcl::make_unexpected(std::move(_gpcl_expected_##var).error());   \
  }                                                                            \
  auto &&var = *_gpcl_expected_##var;

#define GPCL_EXPECTED_TRY_RETURN(exp)                                          \
  ({                                                                           \
    auto tmp = exp;                                                            \
    if (!tmp)                                                                  \
      return ::gpcl::make_unexpected(tmp.error());                             \
    *tmp;                                                                      \
  })

} // namespace gpcl

#include <gpcl/impl/expected.hpp>

#endif // GPCL_EXPECTED_HPP
