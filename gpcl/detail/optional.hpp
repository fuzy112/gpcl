//
// optional.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_OPTIONAL_HPP
#define GPCL_DETAIL_OPTIONAL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/optional_fwd.hpp>
#include <type_traits>

namespace gpcl {

template <typename T>
class optional;

namespace detail {

template <typename T, typename = void>
class optional_destruct_base;
template <typename T, typename = void>
class optional_move_base;
template <typename T, typename = void>
class optional_copy_base;
template <typename T, typename = void>
class optional_move_assign_base;
template <typename T, typename = void>
class optional_copy_assign_base;

template <typename T, typename>
class optional_destruct_base
{
public:
  union
  {
    char dummy_val_;
    T val_;
  };
  bool has_val_ = false;

  constexpr optional_destruct_base() noexcept : dummy_val_(), has_val_() {}

  template <typename... Args,
            typename std::enable_if<std::is_constructible<T, Args...>::value,
                                    int>::type = 0>
  constexpr explicit optional_destruct_base(in_place_t, Args &&... args)
      : val_(detail::forward<Args>(args)...),
        has_val_(true)
  {
  }

  template <typename U, class... Args,
            typename std::enable_if<
                std::is_constructible<T, std::initializer_list<U> &,
                                      Args &&...>::value,
                int>::type = 0>
  constexpr explicit optional_destruct_base(in_place_t,
                                            std::initializer_list<U> ilist,
                                            Args &&... args)
      : val_(ilist, detail::forward<Args>(args)...),
        has_val_(true)
  {
  }

  ~optional_destruct_base() noexcept
  {
    if (has_val_)
      val_.~T();
  }
};

template <typename T>
class optional_destruct_base<
    T, typename std::enable_if<std::is_trivially_destructible<T>::value>::type>
{
public:
  union
  {
    char dummy_val_;
    T val_;
  };
  bool has_val_ = false;

  constexpr optional_destruct_base() noexcept : dummy_val_(), has_val_() {}

  template <typename... Args,
            typename std::enable_if<std::is_constructible<T, Args...>::value,
                                    int>::type = 0>
  constexpr explicit optional_destruct_base(in_place_t, Args &&... args)
      : val_(detail::forward<Args>(args)...),
        has_val_(true)
  {
  }

  template <typename U, class... Args,
            typename std::enable_if<
                std::is_constructible<T, std::initializer_list<U> &,
                                      Args &&...>::value,
                int>::type = 0>
  constexpr explicit optional_destruct_base(in_place_t,
                                            std::initializer_list<U> ilist,
                                            Args &&... args)
      : val_(ilist, detail::forward<Args>(args)...),
        has_val_(true)
  {
  }

  ~optional_destruct_base() = default;
};

template <typename T, typename>
class optional_copy_base : public optional_destruct_base<T>
{
public:
  constexpr optional_copy_base() = default;

  optional_copy_base(const optional_copy_base &other)
  {
    this->has_val_ = other.has_val_;
    if (this->has_val_)
    {
      new (&this->val_) T(this->val_);
    }
  }

  using optional_destruct_base<T>::optional_destruct_base;
};

template <typename T>
class optional_copy_base<
    T, typename std::enable_if<
           std::is_trivially_copy_constructible<T>::value>::type>
    : public optional_destruct_base<T>
{
public:
  constexpr optional_copy_base() = default;
  constexpr optional_copy_base(const optional_copy_base &other) = default;
  using optional_destruct_base<T>::optional_destruct_base;
};

template <typename T>
class optional_copy_base<
    T, typename std::enable_if<!std::is_copy_constructible<T>::value>::type>
    : public optional_destruct_base<T>
{
public:
  constexpr optional_copy_base() = default;
  optional_copy_base(const optional_copy_base &) = delete;
  using optional_destruct_base<T>::optional_destruct_base;
};

template <typename T, typename>
class optional_move_base : public optional_copy_base<T>
{
public:
  constexpr optional_move_base() = default;
  constexpr optional_move_base(const optional_move_base &) = default;

  template <typename U = T,
            typename std::enable_if<std::is_move_constructible<U>::value,
                                    int>::type = 0>
  optional_move_base(optional_move_base &&other) noexcept(
      std::is_nothrow_move_constructible<U>::value)
  {
    this->has_val_ = detail::exchange(other.has_val_, false);
    if (this->has_val_)
    {
      new (&this->val_) T(std::move(other.val_));
      other.val_.T::~T();
    }
  }

  using optional_copy_base<T>::optional_copy_base;
};

template <typename T>
class optional_move_base<
    T, typename std::enable_if<
           std::is_trivially_move_constructible<T>::value>::type>
    : public optional_copy_base<T>
{
public:
  constexpr optional_move_base() = default;
  constexpr optional_move_base(const optional_move_base &) = default;

  constexpr optional_move_base(optional_move_base &&other) = default;
  using optional_copy_base<T>::optional_copy_base;
};

template <typename T, typename>
class optional_copy_assign_base : public optional_move_base<T>
{
public:
  constexpr optional_copy_assign_base() = default;
  constexpr optional_copy_assign_base(const optional_copy_assign_base &) =
      default;
  constexpr optional_copy_assign_base(optional_copy_assign_base &&) = default;

  optional_copy_assign_base &
  operator=(const optional_copy_assign_base &) = delete;

  using optional_move_base<T>::optional_move_base;
};

template <typename T>
class optional_copy_assign_base<
    T,
    typename std::enable_if<std::is_copy_constructible<T>::value &&
                            std::is_copy_assignable<T>::value &&
                            (std::is_trivially_copy_constructible<T>::value &&
                             std::is_trivially_copy_assignable<T>::value &&
                             std::is_trivially_destructible<T>::value)>::type>
    : public optional_move_base<T>
{
public:
  constexpr optional_copy_assign_base() = default;
  constexpr optional_copy_assign_base(const optional_copy_assign_base &) =
      default;
  constexpr optional_copy_assign_base(optional_copy_assign_base &&) = default;

  constexpr optional_copy_assign_base &
  operator=(const optional_copy_assign_base &) = default;

  using optional_move_base<T>::optional_move_base;
};

template <typename T>
class optional_copy_assign_base<
    T, typename std::enable_if<
           std::is_copy_constructible<T>::value &&
           std::is_copy_assignable<T>::value &&
           !((std::is_trivially_copy_constructible<T>::value &&
              std::is_trivially_copy_assignable<T>::value &&
              std::is_trivially_destructible<T>::value))>::type>
    : public optional_move_base<T>
{
public:
  constexpr optional_copy_assign_base() = default;
  constexpr optional_copy_assign_base(const optional_copy_assign_base &) =
      default;
  constexpr optional_copy_assign_base(optional_copy_assign_base &&) = default;

  optional_copy_assign_base &operator=(const optional_copy_assign_base &other)
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

  using optional_move_base<T>::optional_move_base;
};

template <typename T, typename>
class optional_move_assign_base : public optional_copy_assign_base<T>
{
public:
  constexpr optional_move_assign_base() = default;
  constexpr optional_move_assign_base(const optional_move_assign_base &) =
      default;
  constexpr optional_move_assign_base(optional_move_assign_base &&) = default;
  optional_move_assign_base &
  operator=(const optional_move_assign_base &) = default;
  optional_move_assign_base &operator=(optional_move_assign_base &&) = delete;

  using optional_copy_assign_base<T>::optional_copy_assign_base;
};

template <typename T>
class optional_move_assign_base<
    T,
    typename std::enable_if<std::is_move_constructible<T>::value &&
                            std::is_move_assignable<T>::value &&
                            (std::is_trivially_move_constructible<T>::value &&
                             std::is_trivially_move_assignable<T>::value &&
                             std::is_trivially_destructible<T>::value)>::type>
    : public optional_copy_assign_base<T>
{
public:
  constexpr optional_move_assign_base() = default;
  constexpr optional_move_assign_base(const optional_move_assign_base &) =
      default;
  constexpr optional_move_assign_base(optional_move_assign_base &&) = default;
  optional_move_assign_base &
  operator=(const optional_move_assign_base &) = default;

  constexpr optional_move_assign_base &
  operator=(optional_move_assign_base &&) noexcept = default;

  using optional_copy_assign_base<T>::optional_copy_assign_base;
};

template <typename T>
class optional_move_assign_base<
    T,
    typename std::enable_if<std::is_move_constructible<T>::value &&
                            std::is_move_assignable<T>::value &&
                            !(std::is_trivially_move_constructible<T>::value &&
                              std::is_trivially_move_assignable<T>::value &&
                              std::is_trivially_destructible<T>::value)>::type>
    : public optional_copy_assign_base<T>
{
public:
  constexpr optional_move_assign_base() = default;
  constexpr optional_move_assign_base(const optional_move_assign_base &) =
      default;
  constexpr optional_move_assign_base(optional_move_assign_base &&) = default;
  optional_move_assign_base &
  operator=(const optional_move_assign_base &) = default;

  optional_move_assign_base &
  operator=(optional_move_assign_base &&other) noexcept(
      std::is_nothrow_move_constructible<T>::value
          &&std::is_nothrow_move_assignable<T>::value)
  {
    if (this->has_val_ && other.has_val_)
    {
      this->val_ = std::move(other.val_);
      other.val_.T::~T();
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
      other.val_.T::~T();
      other.has_val_ = false;
    }
    else
    {
      // do nothing
    }
    return *this;
  }

  using optional_copy_assign_base<T>::optional_copy_assign_base;
};

template <typename U, typename T>
struct optional_convert_constructible
    : std::integral_constant<
          bool, !std::is_constructible<T, optional<U> &>::value &&
                    !std::is_constructible<T, const optional<U> &>::value &&
                    !std::is_constructible<T, optional<U> &&>::value &&
                    !std::is_constructible<T, const optional<U> &&>::value &&
                    !std::is_convertible<optional<U> &, T>::value &&
                    !std::is_convertible<const optional<U> &, T>::value &&
                    !std::is_convertible<optional<U> &&, T>::value &&
                    !std::is_convertible<const optional<U> &&, T>::value>
{
};

template <typename U, typename T>
struct optional_convert_assignable
    : std::integral_constant<
          bool, !std::is_constructible<T, optional<U> &>::value &&
                    !std::is_constructible<T, const optional<U> &>::value &&
                    !std::is_constructible<T, optional<U> &&>::value &&
                    !std::is_constructible<T, const optional<U> &&>::value &&
                    !std::is_convertible<optional<U> &, T>::value &&
                    !std::is_convertible<const optional<U> &, T>::value &&
                    !std::is_convertible<optional<U> &&, T>::value &&
                    !std::is_convertible<const optional<U> &&, T>::value &&
                    !std::is_assignable<T, optional<U> &>::value &&
                    !std::is_assignable<T, const optional<U> &>::value &&
                    !std::is_assignable<T, optional<U> &&>::value &&
                    !std::is_assignable<T, const optional<U> &&>::value>
{
};

} // namespace detail

} // namespace gpcl

#endif // GPCL_DETAIL_OPTIONAL_HPP
