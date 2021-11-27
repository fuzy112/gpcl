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

namespace gpcl {

struct unexpect_t
{
  explicit unexpect_t() = default;
};

GPCL_CXX17_INLINE_CONSTEXPR unexpect_t unexpect{};

template <typename E>
class unexpected
{
  template <typename T>
  friend class unexpected;

  E val_;

public:
  unexpected() = delete;

  inline constexpr unexpected(const unexpected &) = default;

  inline constexpr unexpected(unexpected &&) noexcept(
      std::is_nothrow_move_constructible<E>::value) = default;

  template <typename... Args,
            std::enable_if_t<detail::is_constructible_v<E, Args...>, int> = 0>
  inline constexpr explicit unexpected(in_place_t, Args &&... args)
      : val_(detail::forward<Args>(args)...)
  {
  }

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

  inline constexpr unexpected &operator=(const unexpected &) = default;
  inline constexpr unexpected &operator=(unexpected &&) noexcept(
      detail::is_nothrow_swappable<E>::value) = default;

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

  inline constexpr const E &value() const & { return val_; }
  inline constexpr E &value() & { return val_; }

  inline constexpr const E &&value() const && { return detail::move(val_); }
  inline constexpr E &&value() && { return detail::move(val_); }

  inline void
  swap(unexpected &other) noexcept(detail::is_nothrow_swappable<E>::value)
  {
    using std::swap;
    swap(val_, other.val_);
  }
};

#if __cplusplus >= 201703
template <typename E>
unexpected(E) -> unexpected<E>;
#endif

template <typename E, std::enable_if_t<detail::is_swappable<E>::value, int> = 0>
inline void
swap(unexpected<E> &lhs,
     unexpected<E> &rhs) noexcept(detail::is_nothrow_swappable<E>::value)
{
  lhs.swap(rhs);
}

/// Create an unexpect value.
template <typename E>
unexpected<typename std::decay<E>::type> make_unexpected(E &&e)
{
  return unexpected<typename std::decay<E>::type>(std::forward<E>(e));
}

/// Create an unexpected<error_code> from error code enum.
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
