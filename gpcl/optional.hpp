//
// optional.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OPTIONAL_HPP
#define GPCL_OPTIONAL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/optional.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/error.hpp>
#include <gpcl/optional_fwd.hpp>
#include <gpcl/swap.hpp>

namespace gpcl {

class bad_optional_access : public std::runtime_error
{
public:
  bad_optional_access() : std::runtime_error("bad_optional_access") {}
};

/// Represents an optional value: either some value T or null.
///
/// @tparam T shall be a type other than *cv* `in_place_t` or *cv* `nullopt_t`
/// that meets the *Cpp17Destructible* requirements.
template <typename T>
class optional : public detail::optional_move_assign_base<T>
{
  using base_type = detail::optional_move_assign_base<T>;

public:
  using value_type = T;

  /// @{
  /// @name Constructors

  /// Default constructor.
  ///
  /// @post *this does not contain a value.
  /// @remarks No contained value is initialized.  For every object type T this
  /// constructor is a constexpr constructor.
  inline constexpr optional() noexcept = default;

  /// @post *this does not contain a value.
  /// @remarks No contained value is initialized.  For every object type T this
  /// constructor is a constexpr constructor.
  inline constexpr optional(nullopt_t) noexcept {}

  /// Copy constructor.
  ///
  /// @details If rhs contains a value, initializes the contained value as if
  /// direct-non-list-initializing an object of type T with the expression *rhs.
  ///
  /// @post bool(rhs) == bool(*this).
  ///
  /// @throws Any exception thrown by the selected constructor of T.
  ///
  /// @remarks This constructor is defined as deleted unless
  /// `is_copy_constructible_v<T>` is true.   If
  /// `is_trivially_copy_constructible_v<T>` is true, this constructor is
  /// trivial.
  inline constexpr optional(const optional &rhs) = default;

  /// Move constructor.
  ///
  /// @pre `is_move_constructible_v<T>` is true.
  ///
  /// @details If `rhs` contains a value, initializes the contained value as if
  /// direct-non-list-initializing an object of type T with the expression
  /// `std::move(*rhs).`  `bool(rhs)` is unchanged.
  ///
  /// @post `bool(rhs) == bool(*this)`
  ///
  /// @throws Any exception thrown by the selected constructor of T.
  ///
  /// @remarks The expression inside `noexcept` is equivalent to
  /// `is_nothrow_move_constructible_v<T>`.  If
  /// `is_trivially_move_constructible_v<T>` is `true`, this constructor is
  /// trivial.
  inline constexpr optional(optional &&rhs) noexcept(
      std::is_nothrow_move_constructible<T>()) = default;

  /// @pre `is_constructible_v<T, Args...>` is `true`.
  ///
  /// @details Initializes the contained value as if
  /// direct-non-list-initializing an object of type T with the arguments
  /// `std::forward<Args>(args)...`.
  ///
  /// @post `*this` contains a value.
  ///
  /// @throws Any exception thrown by the selected constructor of `T`.
  ///
  /// @remarks If `T`'s constructor selected for the initialization is a
  /// constexpr constructor, this constructor is a constexpr constructor.
  template <typename... Args,
            typename std::enable_if<std::is_constructible<T, Args...>::value,
                                    int>::type = 0>
  inline constexpr explicit optional(in_place_t, Args &&... args)
      : base_type(in_place, detail::forward<Args>(args)...)
  {
  }

  /// @pre `is_constructible_v<T, initializer_list<U>&, Args...>` is
  /// `true`.
  ///
  /// @details Initializes the contained value as if
  /// direct-non-list-initializing an object of type T with the arguments
  /// `il, std::forward<Args>(args)...`.
  ///
  /// @post `*this` contains a value.
  ///
  /// @throws Any exception thrown by the selected constructor of `T`.
  ///
  /// @remarks If `T`'s constructor selected for the initialization is a
  /// constexpr constructor, this constructor is a constexpr constructor.
  template <typename U, class... Args,
            std::enable_if_t<std::is_constructible<
                                 T, std::initializer_list<U> &, Args...>::value,
                             int>
                Dummy = 0>
  inline constexpr explicit optional(in_place_t, std::initializer_list<U> il,
                                     Args &&... args)
      : base_type(in_place, il, detail::forward<Args>(args)...)
  {
  }

  /// @pre `is_constructible_v<T, U>` is `true`,
  /// `is_same_v<remove_cvref_t<U>, in_place_t>` is `false`, and
  /// `is_same_v<remove_cvref_t<U>, optional>` is `false`.
  ///
  /// @details Initializes the contained value as if
  /// direct-non-list-initializing an object of type T with the arguments
  /// `std::forward<U>(v)...`.
  ///
  /// @post `*this` contains a value.
  ///
  /// @throws Any exception thrown by the selected constructor of `T`.
  ///
  /// @remarks If `T`'s constructor selected for the initialization is a
  /// constexpr constructor, this constructor is a constexpr constructor.
  /// This constructor is explicit if `is_convertible_v<U, T>` is `false`.
  template <typename U = T,
            std::enable_if_t<
                detail::conjunction_v<
                    detail::negate<std::is_convertible<U, T>>,
                    std::is_constructible<T, U &&>,
                    detail::negate<std::is_same<std::decay_t<U>, in_place_t>>,
                    detail::negate<std::is_same<std::decay_t<U>, optional>>>,
                int>
                Dummy = 0>
  inline explicit constexpr optional(U &&v)
      : base_type(in_place, detail::forward<U>(v))
  {
  }

  /// @pre `is_constructible_v<T, U>` is `true`,
  /// `is_same_v<remove_cvref_t<U>, in_place_t>` is `false`, and
  /// `is_same_v<remove_cvref_t<U>, optional>` is `false`.
  ///
  /// @details Initializes the contained value as if
  /// direct-non-list-initializing an object of type T with the arguments
  /// `std::forward<U>(v)...`.
  ///
  /// @post `*this` contains a value.
  ///
  /// @throws Any exception thrown by the selected constructor of `T`.
  ///
  /// @remarks If `T`'s constructor selected for the initialization is a
  /// constexpr constructor, this constructor is a constexpr constructor.
  /// This constructor is explicit if `is_convertible_v<U, T>` is `false`.
  template <typename U = T,
            std::enable_if_t<
                detail::conjunction_v<
                    std::is_convertible<U, T>, std::is_constructible<T, U &&>,
                    detail::negate<std::is_same<std::decay_t<U>, in_place_t>>,
                    detail::negate<std::is_same<std::decay_t<U>, optional>>>,
                int>
                Dummy1 = 0>
  inline constexpr optional(U &&v) : base_type(in_place, detail::forward<U>(v))
  {
  }

  /// @pre
  ///  `is_constructible_v<T, const U &>` is `true`,
  ///  `is_constructible_v<T, optional<U>&>` is `false`,
  ///  `is_constructible_v<T, optional<U>&&>` is `false`,
  ///  `is_constructible_v<T, const optional<U>&>` is `false`,
  ///  `is_constructible_v<T, const optional<U>&&>` is `false`,
  ///  `is_convertible_v<optional<U>&, T>` is `false`,
  ///  `is_convertible_v<optional<U>&&, T>` is `false`,
  ///  `is_convertible_v<const optional<U>&, T>` is `false`, and
  ///  `is_convertible_v<const optional<U>&&, T>` is `false`.
  ///
  /// @details If `rhs` contains a value, initializes the contained value as if
  /// direct-non-list-initializing an object of type `T` with the expression
  /// `*rhs`.
  ///
  /// @post `bool(rhs) == bool(*this)`.
  ///
  /// @throws Any exception thrown by the selected constructor of T.
  ///
  /// @remarks This constructor is explicit iff `is_convertible_v<const U&, T>`
  /// is `false`.
  template <
      typename U,
      std::enable_if_t<detail::conjunction_v<
                           std::is_constructible<T, const U &>,
                           detail::optional_convert_constructible<U, T>,
                           detail::negate<std::is_convertible<const U &, T>>>,
                       int>
          Dummy = 0>
  inline explicit optional(const optional<U> &other)
  {
    this->has_val_ = other.has_val_;
    if (this->has_val_)
    {
      new (&this->val_) T(other.val_);
    }
  }

  template <typename U>
  inline optional(const optional<U> &other,
                  typename std::enable_if<
                      std::is_constructible<T, const U &>::value &&
                          detail::optional_convert_constructible<U, T>::value &&
                          std::is_convertible<const U &, T>::value,
                      int>::type = 0)
  {
    this->has_val_ = other.has_val_;
    if (this->has_val_)
    {
      new (&this->val_) T(other.val_);
    }
  }

  template <typename U>
  inline explicit optional(
      optional<U> &&other,
      typename std::enable_if<
          std::is_constructible<T, U &&>::value &&
              detail::optional_convert_constructible<U, T>::value &&
              !std::is_convertible<U &&, T>::value,
          int>::type = 0)
  {
    this->has_val_ = detail::exchange(other.has_val_, false);
    if (this->has_val_)
    {
      new (&this->val_) T(other.val_);
    }
  }

  template <typename U>
  inline optional(optional<U> &&other,
                  typename std::enable_if<
                      std::is_constructible<T, U &&>::value &&
                          detail::optional_convert_constructible<U, T>::value &&
                          std::is_convertible<U &&, T>::value,
                      std::nullptr_t>::type = nullptr)
  {
    this->has_val_ = detail::exchange(other.has_val_, false);
    if (this->has_val_)
    {
      new (&this->val_) T(other.val_);
    }
  }

  /// @}

  /// @name Destructor
  inline ~optional() = default;

  /// @name Assignment Operators
  /// @{

  inline optional &operator=(nullopt_t) noexcept
  {
    if (detail::exchange(this->has_val_, false))
    {
      this->val_.T::~T();
    }
    return *this;
  }

  inline constexpr optional &operator=(const optional &other) = default;

  inline constexpr optional &operator=(optional &&other) noexcept(
      detail::is_nothrow_move_constructible<T>::value
          &&detail::is_nothrow_move_assignable<T>::value) = default;

  template <typename U = T>
  inline auto operator=(U &&u) -> typename std::enable_if<
      std::is_constructible<T, U>::value && std::is_assignable<T &, U>::value &&
          (!std::is_scalar<T>::value ||
           !std::is_same<typename std::decay<U>::type, T>::value),
      optional &>::type
  {
    if (this->has_val_)
    {
      this->val_ = detail::forward<U>(u);
    }
    else
    {
      new (&this->val_) T(detail::forward<U>(u));
      this->has_val_ = true;
    }
    return *this;
  }

  template <typename U>
  inline auto operator=(const optional<U> &other) -> typename std::enable_if<
      detail::optional_convert_assignable<U, T>::value &&
          std::is_constructible<T, const U &>::value &&
          std::is_assignable<T &, const U &>::value,
      optional &>::type
  {
    if (this->has_val_ && other.has_val_)
    {
      this->val_ = other.val_;
    }
    else if (this->has_val_ && !other.has_val_)
    {
      this->val_.T::~T();
      this->has_val_ = false;
    }
    else if (!this->has_val_ && other.has_val_)
    {
      this->has_val_ = true;
      new (&this->val_) T(other.val_);
    }
    else
    {
      // do nothing
    }
    return *this;
  }

  template <typename U>
  inline auto operator=(optional<U> &&other) -> typename std::enable_if<
      detail::optional_convert_assignable<U, T>::value &&
          std::is_constructible<T, U &&>::value &&
          std::is_assignable<T &, U>::value,
      optional &>::type
  {
    if (this->has_val_ && other.has_val_)
    {
      this->val_ = std::move(other.val_);
      other.has_val_ = false;
    }
    else if (this->has_val_ && !other.has_val_)
    {
      this->val_.T::~T();
      this->has_val_ = false;
    }
    else if (!this->has_val_ && other.has_val_)
    {
      this->has_val_ = true;
      new (&this->val_) T(std::move(other.val_));
      other.has_val_ = false;
    }
    else
    {
      // do nothing
    }
    return *this;
  }

  /// @}

  /// @name Modifiers
  /// @{

  template <typename... Args>
  T &emplace(Args &&... args)
  {
    if (this->has_val_ && !std::is_trivially_destructible<T>())
    {
      this->val_.T::~T();
      this->has_val_ = false;
    }
    new (&this->val_) T(detail::forward<Args>(args)...);
    this->has_val_ = true;
    return this->val_;
  }

  template <typename U, class... Args>
  T &emplace(std::initializer_list<U> ilist, Args &&... args)
  {
    if (this->has_val_ && !std::is_trivially_destructible<T>())
    {
      this->val_.T::~T();
      this->has_val_ = false;
    }
    new (&this->val_) T(ilist, detail::forward<Args>(args)...);
    this->has_val_ = true;
    return this->val_;
  }

  // swap
  void swap(optional &other) noexcept(
      detail::is_nothrow_swappable<T>::value
          &&detail::is_nothrow_move_constructible<T>::value)
  {
    if (this->has_val_ && other.has_val_)
    {
      gpcl::swap(this->val_, other.val_);
    }
    else if (this->has_val_ && !other.has_val_)
    {
      other = detail::move(*this);
      this->val_.T::~T();
      this->has_val_ = false;
      other.has_val_ = true;
    }
    else if (!this->has_val_ && other.has_val_)
    {
      *this = detail::move(other);
      other.val_.T::~T();
      other.has_val_ = false;
      this->has_val_ = true;
    }
  }

  /// @}

  /// @name Observers
  /// @{

  constexpr const T *operator->() const
  {
    GPCL_ASSERT_CONST(this->has_val_);
    return &this->val_;
  }

  constexpr T *operator->()
  {
    GPCL_ASSERT_CONST(this->has_val_);
    return &this->val_;
  }

  constexpr const T &operator*() const &
  {
    GPCL_ASSERT_CONST(this->has_val_);
    return this->val_;
  }

  constexpr T &operator*() &
  {
    GPCL_ASSERT_CONST(this->has_val_);
    return this->val_;
  }

  constexpr T &&operator*() &&
  {
    GPCL_ASSERT_CONST(this->has_val_);
    return detail::move(this->val_);
  }

  constexpr const T &&operator*() const &&
  {
    GPCL_ASSERT_CONST(this->has_val_);
    return detail::move(this->val_);
  }

  constexpr explicit operator bool() const noexcept { return this->has_val_; }

  [[nodiscard]] constexpr bool has_value() const noexcept
  {
    return this->has_val_;
  }

  constexpr const T &value() const &
  {
    if (has_value())
    {
      return **this;
    }

    GPCL_THROW(bad_optional_access());
  }

  constexpr T &value() &
  {
    if (has_value())
    {
      return **this;
    }

    GPCL_THROW(bad_optional_access());
  }

  constexpr T &&value() &&
  {
    if (has_value())
    {
      return detail::move(**this);
    }

    GPCL_THROW(bad_optional_access());
  }
  constexpr const T &&value() const &&
  {
    if (has_value())
    {
      return detail::move(**this);
    }

    GPCL_THROW(bad_optional_access());
  }

  template <typename U>
  constexpr T value_or(U &&default_value) const &
  {
    return bool(*this) ? **this
                       : static_cast<T>(detail::forward<U>(default_value));
  }

  template <typename U>
  constexpr T value_or(U &&default_value) &&
  {
    return bool(*this) ? std::move(**this)
                       : static_cast<T>(detail::forward<U>(default_value));
  }

  /// @}

  /// @name Modifiers
  /// @{
  void reset() noexcept { *this = nullopt; }
  /// @}

  /// @name Extensions
  /// @{

  template <typename U>
  optional<U> and_(const optional<U> &x) const &;

  template <typename U>
  optional<U> and_(optional<U> &&x) const &;

  template <typename U>
  optional<U> and_(const optional<U> &x) &&;

  template <typename U>
  optional<U> and_(optional<U> &&x) &&;

  /// @}
};

#if __cplusplus >= 201703 && !defined(GPCL_STANDARDESE)
template <typename T>
optional(T) -> optional<T>;
#endif

/// @name Relational Operators
/// @{

template <typename T, class U>
inline constexpr bool operator==(const optional<T> &x, const optional<U> &y)
{
  if (x && y)
    return *x == *y;

  if (!x && !y)
    return true;

  return false;
}

template <typename T, class U>
inline constexpr bool operator!=(const optional<T> &x, const optional<U> &y)
{
  return !(x == y);
}

template <typename T, class U>
inline constexpr bool operator<(const optional<T> &x, const optional<U> &y)
{
  if (x && y)
    return *x < *y;
  return false;
}

template <typename T, class U>
inline constexpr bool operator>(const optional<T> &x, const optional<U> &y)
{
  if (x && y)
    return *x > *y;
  return false;
}

template <typename T, class U>
inline constexpr bool operator<=(const optional<T> &x, const optional<U> &y)
{
  if (x && y)
    return *x <= *y;
  return false;
}

template <typename T, class U>
inline constexpr bool operator>=(const optional<T> &x, const optional<U> &y)
{
  if (x && y)
    return *x >= *y;
  return false;
}

template <typename T>
inline constexpr bool operator==(const optional<T> &x, nullopt_t) noexcept
{
  return !x.has_value();
}

// comparison with T
template <typename T, class U>
inline constexpr bool operator==(const optional<T> &x, const U &y)
{
  return x && (*x == y);
}

template <typename T, class U>
inline constexpr bool operator==(const T &x, const optional<U> &y)
{
  return y && (x == *y);
}

template <typename T, class U>
inline constexpr bool operator!=(const optional<T> &x, const U &y)
{
  return !(x == y);
}

template <typename T, class U>
inline constexpr bool operator!=(const T &x, const optional<U> &y)
{
  return !(x == y);
}

template <typename T, class U>
inline constexpr bool operator<(const optional<T> &x, const U &y)
{
  return x && (*x < y);
}

template <typename T, class U>
inline constexpr bool operator<(const T &x, const optional<U> &y)
{
  return y && (x < *y);
}

template <typename T, class U>
inline constexpr bool operator>(const optional<T> &x, const U &y)
{
  return x && (*x > y);
}

template <typename T, class U>
inline constexpr bool operator>(const T &x, const optional<U> &y)
{
  return y && (x > *y);
}

template <typename T, class U>
inline constexpr bool operator<=(const optional<T> &x, const U &y)
{
  return x && (*x <= y);
}

template <typename T, class U>
inline constexpr bool operator<=(const T &x, const optional<U> &y)
{
  return y && (x <= *y);
}

template <typename T, class U>
inline constexpr bool operator>=(const optional<T> &x, const U &y)
{
  return x && (*x >= y);
}

template <typename T, class U>
inline constexpr bool operator>=(const T &x, const optional<U> &y)
{
  return y && (x > *y);
}

/// @}

namespace swap_detail {
// specialized algorithms
template <typename T>
void swap(optional<T> &x, optional<T> &y) noexcept(true)
{
  x.swap(y);
}
} // namespace swap_detail

/// @relates gpcl::optional
template <typename T>
constexpr optional<typename std::decay<T>::type> make_optional(T &&x)
{
  return optional<typename std::decay<T>::type>(detail::forward<T>(x));
}

/// @relates gpcl::optional
template <typename T, class... Args>
constexpr optional<T> make_optional(Args &&... args)
{
  return optional<T>(in_place, detail::forward<Args>(args)...);
}

/// @relates gpcl::optional
template <typename T, class U, class... Args>
constexpr optional<T> make_optional(std::initializer_list<U> il,
                                    Args &&... args)
{
  return optional<T>(in_place, il, detail::forward<Args>(args)...);
}

} // namespace gpcl

#endif // GPCL_OPTIONAL_HPP
