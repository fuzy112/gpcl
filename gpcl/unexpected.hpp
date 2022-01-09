//
// unexpected.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_UNEXPECTED_HPP
#define GPCL_UNEXPECTED_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/error.hpp>
#include <gpcl/expected_fwd.hpp>
#include <gpcl/in_place.hpp>
#include <gpcl/swap.hpp>

namespace gpcl {

struct unexpect_t
{
  explicit unexpect_t() = default;
};

constexpr unexpect_t unexpect{};

/// Used as a wrapper to store the unexpected value.
/// @tparam E must not be void.
template <typename E>
class unexpected
{
  template <typename T>
  friend class unexpected;

  E val_;

public:
  /// @name Constructors
  /// @{

  unexpected() = delete;

  /// Copies the unexpected value.
  inline constexpr unexpected(const unexpected &) = default;

  /// Moves the unexpected value.
  inline constexpr unexpected(unexpected &&) noexcept(
      std::is_nothrow_move_constructible<E>::value) = default;

  /// Constructs an unexpected value in place.
  template <typename... Args,
            std::enable_if_t<detail::is_constructible_v<E, Args...>, int> = 0>
  inline constexpr explicit unexpected(in_place_t, Args &&... args)
      : val_(detail::forward<Args>(args)...)
  {
  }

  /// Constructs an unexpected value in place.
  template <typename U, typename... Args,
            std::enable_if_t<detail::is_constructible_v<
                                 E, std::initializer_list<U> &, Args...>,
                             int> = 0>
  inline constexpr explicit unexpected(in_place_t,
                                       std::initializer_list<U> ilist,
                                       Args &&... args)
      : val_(ilist, detail::forward<Args>(args)...)
  {
  }

  /// Creates a wrapper for the given unexpected value.
  template <
      typename Err = E,
      std::enable_if_t<detail::is_constructible_v<E, Err> &&
                           !detail::is_same_v<std::decay_t<Err>, in_place_t> &&
                           !detail::is_same_v<std::decay_t<Err>, unexpected>,
                       int> = 0>
  inline constexpr explicit unexpected(Err &&err)
      : val_(detail::forward<Err>(err))
  {
  }

  /// Converts the unexpected value.
  template <typename Err,
            std::enable_if_t<detail::is_convertible_v<Err, Err>, int> = 0,
            std::enable_if_t<
                detail::is_constructible_v<E, Err> &&
                    !detail::is_constructible_v<E, unexpected<Err> &> &&
                    !detail::is_constructible_v<E, unexpected<Err>> &&
                    !detail::is_constructible_v<E, const unexpected<Err> &> &&
                    !detail::is_constructible_v<E, const unexpected<Err>> &&
                    !detail::is_convertible_v<unexpected<Err> &, E> &&
                    !detail::is_convertible_v<unexpected<Err>, E> &&
                    !detail::is_convertible_v<const unexpected<Err> &, E> &&
                    !detail::is_convertible_v<const unexpected<Err>, E>,
                int>
                Dummy1 = 0>
  inline constexpr unexpected(const unexpected<Err> &e) : val_(e.val_)
  {
  }

  /// Converts the unexpected value.
  template <typename Err,
            std::enable_if_t<!detail::is_convertible_v<Err, Err>, int> = 0,
            std::enable_if_t<
                detail::is_constructible_v<E, Err> &&
                    !detail::is_constructible_v<E, unexpected<Err> &> &&
                    !detail::is_constructible_v<E, unexpected<Err>> &&
                    !detail::is_constructible_v<E, const unexpected<Err> &> &&
                    !detail::is_constructible_v<E, const unexpected<Err>> &&
                    !detail::is_convertible_v<unexpected<Err> &, E> &&
                    !detail::is_convertible_v<unexpected<Err>, E> &&
                    !detail::is_convertible_v<const unexpected<Err> &, E> &&
                    !detail::is_convertible_v<const unexpected<Err>, E>,
                int>
                Dummy2 = 0>
  inline constexpr explicit unexpected(const unexpected<Err> &e) : val_(e.val_)
  {
  }

  /// Converts the unexpected value.
  template <typename Err,
            std::enable_if_t<detail::is_convertible_v<Err, Err>, int> = 0,
            std::enable_if_t<
                detail::is_constructible_v<E, Err> &&
                    !detail::is_constructible_v<E, unexpected<Err> &> &&
                    !detail::is_constructible_v<E, unexpected<Err>> &&
                    !detail::is_constructible_v<E, const unexpected<Err> &> &&
                    !detail::is_constructible_v<E, const unexpected<Err>> &&
                    !detail::is_convertible_v<unexpected<Err> &, E> &&
                    !detail::is_convertible_v<unexpected<Err>, E> &&
                    !detail::is_convertible_v<const unexpected<Err> &, E> &&
                    !detail::is_convertible_v<const unexpected<Err>, E>,
                int>
                Dummy1 = 0>
  inline constexpr unexpected(unexpected<Err> &&e) : val_(detail::move(e.val_))
  {
  }

  /// Converts the unexpected value.
  template <typename Err,
            std::enable_if_t<!detail::is_convertible_v<Err, Err>, int> = 0,
            std::enable_if_t<
                detail::is_constructible_v<E, Err> &&
                    !detail::is_constructible_v<E, unexpected<Err> &> &&
                    !detail::is_constructible_v<E, unexpected<Err>> &&
                    !detail::is_constructible_v<E, const unexpected<Err> &> &&
                    !detail::is_constructible_v<E, const unexpected<Err>> &&
                    !detail::is_convertible_v<unexpected<Err> &, E> &&
                    !detail::is_convertible_v<unexpected<Err>, E> &&
                    !detail::is_convertible_v<const unexpected<Err> &, E> &&
                    !detail::is_convertible_v<const unexpected<Err>, E>,
                int>
                Dummy2 = 0>
  inline constexpr explicit unexpected(unexpected<Err> &&e)
      : val_(detail::move(e.val_))
  {
  }

  /// @}

  /// @name Assignment Operators
  /// @{

  /// Copies the unexpected value.
  inline constexpr unexpected &operator=(const unexpected &) = default;

  /// Moves the unexpected value.
  inline constexpr unexpected &operator=(unexpected &&) noexcept(
      detail::is_nothrow_swappable<E>::value) = default;

  /// Converts the unexpected value.
  template <typename Err,
            std::enable_if_t<
                detail::is_assignable_v<E, Err> &&
                    !detail::is_assignable_v<E, unexpected<Err> &> &&
                    !detail::is_assignable_v<E, unexpected<Err>> &&
                    !detail::is_assignable_v<E, const unexpected<Err> &> &&
                    !detail::is_assignable_v<E, const unexpected<Err>> &&
                    !detail::is_convertible_v<unexpected<Err> &, E> &&
                    !detail::is_convertible_v<unexpected<Err>, E> &&
                    !detail::is_convertible_v<const unexpected<Err> &, E> &&
                    !detail::is_convertible_v<const unexpected<Err>, E>,
                int> = 0>
  inline constexpr unexpected &operator=(const unexpected<Err> &other)
  {
    val_ = other.val_;
    return *this;
  }

  /// Converts the unexpected value.
  template <typename Err,
            std::enable_if_t<
                detail::is_assignable_v<E, Err> &&
                    !detail::is_assignable_v<E, unexpected<Err> &> &&
                    !detail::is_assignable_v<E, unexpected<Err>> &&
                    !detail::is_assignable_v<E, const unexpected<Err> &> &&
                    !detail::is_assignable_v<E, const unexpected<Err>> &&
                    !detail::is_convertible_v<unexpected<Err> &, E> &&
                    !detail::is_convertible_v<unexpected<Err>, E> &&
                    !detail::is_convertible_v<const unexpected<Err> &, E> &&
                    !detail::is_convertible_v<const unexpected<Err>, E>,
                int> = 0>
  inline constexpr unexpected &operator=(unexpected<Err> &&other)
  {
    val_ = detail::move(other.val_);
    return *this;
  }

  /// @}

  /// @name Obversers
  /// Accesses the unexpected value.
  /// @{

  inline constexpr const E &value() const & { return val_; }
  inline constexpr E &value() & { return val_; }

  inline constexpr const E &&value() const && { return detail::move(val_); }
  inline constexpr E &&value() && { return detail::move(val_); }

  /// @}

  /// Swaps with another unexpected value.
  inline void
  swap(unexpected &other) noexcept(detail::is_nothrow_swappable<E>::value)
  {
    swap(val_, other.val_);
  }
};

#if __cplusplus >= 201703
template <typename E>
unexpected(E) -> unexpected<E>;
#endif

/// @name Comparators
/// Compares two unexpected objects by comparing their stored value.
/// @relates gpcl::unexpected
/// @{

template <class E>
constexpr bool operator==(const unexpected<E> &lhs, const unexpected<E> &rhs)
{
  return lhs.value() == rhs.value();
}

template <class E>
constexpr bool operator!=(const unexpected<E> &lhs, const unexpected<E> &rhs)
{
  return lhs.value() != rhs.value();
}

template <class E>
constexpr bool operator<(const unexpected<E> &lhs, const unexpected<E> &rhs)
{
  return lhs.value() < rhs.value();
}

template <class E>
constexpr bool operator<=(const unexpected<E> &lhs, const unexpected<E> &rhs)
{
  return lhs.value() <= rhs.value();
}
template <class E>
constexpr bool operator>(const unexpected<E> &lhs, const unexpected<E> &rhs)
{
  return lhs.value() > rhs.value();
}

template <class E>
constexpr bool operator>=(const unexpected<E> &lhs, const unexpected<E> &rhs)
{
  return lhs.value() >= rhs.value();
}

/// @}

namespace detail {
template <typename E, std::enable_if_t<detail::is_swappable<E>::value, int> = 0>
inline void
swap(unexpected<E> &lhs,
     unexpected<E> &rhs) noexcept(detail::is_nothrow_swappable<E>::value)
{
  lhs.swap(rhs);
}
} // namespace detail

/// Create an unexpect value.
/// @relates gpcl::unexpected
template <typename E>
unexpected<typename std::decay<E>::type> make_unexpected(E &&e)
{
  return unexpected<typename std::decay<E>::type>(std::forward<E>(e));
}

/// Create an unexpected<error_code> from error code enum.
/// @relates gpcl::unexpected
template <typename ErrC>
unexpected<error_code> make_unexpected_error_code(ErrC e)
{
  static_assert(std::is_error_code_enum<ErrC>() ||
                    std::is_error_condition_enum<ErrC>(),
                "ErrC shall be error code enum");
  using std::make_error_code;
  return make_unexpected(make_error_code(e));
}

} // namespace gpcl

#endif
