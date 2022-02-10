//
// expected.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_EXPECTED_HPP
#define GPCL_DETAIL_EXPECTED_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/unexpected.hpp>

namespace gpcl {
namespace detail {

#ifndef GPCL_DOXYGEN
template <typename T, typename E, bool TIsCVVoid = detail::is_void_v<T>,
          bool BothAreTriviallyDestructible =
              (detail::is_trivially_destructible_v<T> &&
               detail::is_trivially_destructible_v<E>)>
class expected_destruct_base;

template <typename T, typename E>
class expected_destruct_base<T, E, false, true>
{
protected:
  union
  {
    T val_;
    unexpected<E> err_;
  };
  bool ok_;

public:
  template <typename U = T,
            std::enable_if_t<detail::is_default_constructible_v<U>, int> = 0>
  inline constexpr expected_destruct_base() : val_(),
                                              ok_(true)
  {
  }

  template <typename... Args>
  inline explicit constexpr expected_destruct_base(in_place_t, Args &&... args)
      : val_(detail::forward<Args>(args)...),
        ok_(true)
  {
  }

  template <typename U, typename... Args>
  inline constexpr expected_destruct_base(in_place_t,
                                          std::initializer_list<U> il,
                                          Args &&... args)
      : val_(il, detail::forward<Args>(args)...),
        ok_(true)
  {
  }

  template <typename... Args>
  inline explicit constexpr expected_destruct_base(unexpect_t, Args &&... args)
      : err_(detail::forward<Args>(args)...),
        ok_(false)
  {
  }

  template <typename U, typename... Args>
  inline constexpr expected_destruct_base(unexpect_t,
                                          std::initializer_list<U> il,
                                          Args &&... args)
      : err_(il, detail::forward<Args>(args)...),
        ok_(false)
  {
  }

  constexpr expected_destruct_base(const expected_destruct_base &) = default;
  constexpr expected_destruct_base(expected_destruct_base &&) = default;

  expected_destruct_base &operator=(const expected_destruct_base &) = default;
  expected_destruct_base &operator=(expected_destruct_base &&) = default;

  ~expected_destruct_base() noexcept = default;
};

template <typename T, typename E>
class expected_destruct_base<T, E, false, false>
{
protected:
  union
  {
    T val_;
    unexpected<E> err_;
  };
  bool ok_;

public:
  template <typename U = T,
            std::enable_if_t<detail::is_default_constructible_v<U>, int> = 0>
  inline constexpr expected_destruct_base() : val_(),
                                              ok_(true)
  {
  }

  template <typename... Args>
  inline explicit constexpr expected_destruct_base(in_place_t, Args &&... args)
      : val_(detail::forward<Args>(args)...),
        ok_(true)
  {
  }

  template <typename U, typename... Args>
  inline constexpr expected_destruct_base(in_place_t,
                                          std::initializer_list<U> il,
                                          Args &&... args)
      : val_(il, detail::forward<Args>(args)...),
        ok_(true)
  {
  }

  template <typename... Args>
  inline explicit constexpr expected_destruct_base(unexpect_t, Args &&... args)
      : err_(detail::forward<Args>(args)...),
        ok_(false)
  {
  }

  template <typename U, typename... Args>
  inline constexpr expected_destruct_base(unexpect_t,
                                          std::initializer_list<U> il,
                                          Args &&... args)
      : err_(il, detail::forward<Args>(args)...),
        ok_(false)
  {
  }

  constexpr expected_destruct_base(const expected_destruct_base &) = default;
  constexpr expected_destruct_base(expected_destruct_base &&) = default;

  expected_destruct_base &operator=(const expected_destruct_base &) = default;
  expected_destruct_base &operator=(expected_destruct_base &&) = default;

  inline ~expected_destruct_base() noexcept
  {
    if (ok_)
    {
      GPCL_CXX17_IF_CONSTEXPR(!detail::is_trivially_destructible_v<T>)
      {
        val_.T::~T();
      }
    }
    else
    {
      GPCL_CXX17_IF_CONSTEXPR(!detail::is_trivially_destructible_v<E>)
      {
        err_.unexpected<E>::~unexpected();
      }
    }
  }
};

template <typename T, typename E>
class expected_destruct_base<T, E, true, false>
{
protected:
  union
  {
    char dummy_val_{};
    unexpected<E> err_;
  };
  bool ok_ : 1;

public:
  inline constexpr expected_destruct_base() noexcept : dummy_val_(), ok_(true)
  {
  }

  template <typename... Args>
  inline explicit constexpr expected_destruct_base(unexpect_t, Args &&... args)
      : err_(detail::forward<Args>(args)...),
        ok_(false)
  {
  }

  template <typename U, typename... Args>
  inline constexpr expected_destruct_base(unexpect_t,
                                          std::initializer_list<U> il,
                                          Args &&... args)
      : err_(il, detail::forward<Args>(args)...),
        ok_(false)
  {
  }

  constexpr expected_destruct_base(const expected_destruct_base &) = default;
  constexpr expected_destruct_base(expected_destruct_base &&) = default;

  expected_destruct_base &operator=(const expected_destruct_base &) = default;
  expected_destruct_base &operator=(expected_destruct_base &&) = default;

  inline ~expected_destruct_base() noexcept
  {
    if (!ok_)
    {
      GPCL_CXX17_IF_CONSTEXPR(!detail::is_trivially_destructible_v<E>)
      {
        err_.unexpected<E>::~unexpected();
      }
    }
  }
};

template <typename T, typename E>
class expected_error_base;

template <typename T, typename E>
class expected_error_base : public expected_destruct_base<T, E>
{
public:
  using expected_destruct_base<T, E>::expected_destruct_base;

  constexpr expected_error_base() = default;

  constexpr expected_error_base(const expected_error_base &) = default;
  constexpr expected_error_base(expected_error_base &&) = default;

  expected_error_base &operator=(const expected_error_base &) = default;
  expected_error_base &operator=(expected_error_base &&) = default;

  ~expected_error_base() noexcept = default;

  inline constexpr explicit operator bool() const { return this->ok_; }
  [[nodiscard]] inline constexpr bool has_value() const noexcept
  {
    return this->ok_;
  }

  inline constexpr const E &error() const &
  {
    GPCL_ASSERT(!*this);
    return this->err_.value();
  }
  inline constexpr E &error() &
  {
    GPCL_ASSERT(!*this);
    return this->err_.value();
  }
  inline constexpr E &&error() &&
  {
    GPCL_ASSERT(!*this);
    return detail::move(this->err_.value());
  }
  inline constexpr const E &&error() const &&
  {
    GPCL_ASSERT(!*this);
    return detail::move(this->err_.value());
  }
};

template <typename T, typename E, bool TIsCVVoid = detail::is_void_v<T>>
class expected_value_base;

template <typename T, typename E>
class expected_value_base<T, E, false> : public expected_error_base<T, E>
{
public:
  using expected_error_base<T, E>::expected_error_base;

  constexpr expected_value_base() = default;

  constexpr expected_value_base(const expected_value_base &) = default;
  constexpr expected_value_base(expected_value_base &&) = default;

  expected_value_base &operator=(const expected_value_base &) = default;
  expected_value_base &operator=(expected_value_base &&) = default;

  ~expected_value_base() noexcept = default;

  inline constexpr const T *operator->() const
  {
    GPCL_ASSERT(this->ok_);
    return &this->val_;
  }

  inline T *operator->()
  {
    GPCL_ASSERT(this->ok_);
    return &this->val_;
  }

  inline T &operator*() &
  {
    GPCL_ASSERT(this->ok_);
    return this->val_;
  }

  inline constexpr const T &operator*() const &
  {
    GPCL_ASSERT(this->ok_);
    return this->val_;
  }

  inline constexpr T &&operator*() &&
  {
    GPCL_ASSERT(this->ok_);
    return detail::move(this->val_);
  }

  inline constexpr const T &&operator*() const &&
  {
    GPCL_ASSERT(this->ok_);
    return detail::move(this->val_);
  }

  inline constexpr const T &value() const &
  {
    if (!this->ok_)
    {
      GPCL_THROW(bad_expected_access<E>(this->error()));
    }
    return **this;
  }

  inline constexpr T &value() &
  {
    if (!this->ok_)
    {
      GPCL_THROW(bad_expected_access<E>(this->error()));
    }
    return **this;
  }

  inline constexpr T &&value() &&
  {
    if (!this->ok_)
    {
      GPCL_THROW(bad_expected_access<E>(this->error()));
    }
    return detail::move(**this);
  }

  inline constexpr const T &&value() const &&
  {
    if (!this->ok_)
    {
      GPCL_THROW(bad_expected_access<E>(this->error()));
    }
    return detail::move(**this);
  }

  template <class U>
  inline constexpr T value_or(U &&v) const &
  {
    static_assert(!(detail::is_copy_constructible_v<T> &&
                    !detail::is_convertible_v<U, T>));
    return bool(*this) ? **this : static_cast<T>(detail::forward<U>(v));
  }

  template <class U>
  inline constexpr T value_or(U &&v) &&
  {
    static_assert(!(detail::is_move_constructible_v<T> &&
                    !detail::is_convertible_v<U, T>));
    return bool(*this) ? detail::move(**this)
                       : static_cast<T>(detail::forward<U>(v));
  }

  template <typename... Args,
            std::enable_if_t<detail::is_constructible_v<T, Args...>, int> = 0>
  inline T &emplace(Args &&... args)
  {
    if (*this)
    {
      this->val_ = T(detail::forward<Args>(args)...);
    }
    else
    {
      GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_constructible_v<T, Args...>)
      {
        this->err_.unexpected<E>::~unexpected();
        ::new (&this->val_) T(detail::forward<Args>(args)...);
        this->ok_ = true;
      }
      else GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_move_constructible_v<T>)
      {
        T tmp = T(detail::forward<Args>(args)...);
        this->err_.unexpected<E>::~unexpected();
        ::new (&this->val_) T(detail::move(tmp));
        this->ok_ = true;
      }
      else
      {
        unexpected<E> tmp(detail::move(this->error()));
        this->err_.unexpected<E>::~unexpected();
        GPCL_TRY { ::new (&this->val_) T(detail::forward<Args>(args)...); }
        GPCL_CATCH(...)
        {
          ::new (&this->err_) unexpected<E>(detail::move(tmp));
          GPCL_RETHROW;
        }
        GPCL_CATCH_END
        this->ok_ = true;
      }
    }
    return **this;
  }

  template <typename... Args, typename U,
            std::enable_if_t<detail::is_constructible_v<
                                 T, std::initializer_list<U> &, Args...>,
                             int> = 0>
  inline T &emplace(std::initializer_list<U> il, Args &&... args)
  {
    if (*this)
    {
      this->val_ = T(il, detail::forward<Args>(args)...);
    }
    else
    {
      GPCL_CXX17_IF_CONSTEXPR(
          detail::is_nothrow_constructible_v<T, std::initializer_list<U> &,
                                             Args...>)
      {
        this->err_.unexpected<E>::~unexpected();
        ::new (&this->val_) T(il, detail::forward<Args>(args)...);
        this->ok_ = true;
      }
      else GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_move_constructible_v<T>)
      {
        T tmp = T(il, detail::forward<Args>(args)...);
        this->err_.unexpected<E>::~unexpected();
        ::new (&this->val_) T(detail::move(tmp));
        this->ok_ = true;
      }
      else
      {
        unexpected<E> tmp(detail::move(this->error()));
        this->err_.unexpected<E>::~unexpected();
        GPCL_TRY { ::new (&this->val_) T(il, detail::forward<Args>(args)...); }
        GPCL_CATCH(...)
        {
          ::new (&this->err_) unexpected<E>(detail::move(tmp));
          GPCL_RETHROW;
        }
        GPCL_CATCH_END
        this->ok_ = true;
      }
    }
    return **this;
  }
};

template <typename T, typename E>
class expected_value_base<T, E, true> : public expected_error_base<T, E>
{
public:
  using expected_error_base<T, E>::expected_error_base;

  constexpr expected_value_base() = default;

  constexpr expected_value_base(const expected_value_base &) = default;
  constexpr expected_value_base(expected_value_base &&) = default;

  expected_value_base &operator=(const expected_value_base &) = default;
  expected_value_base &operator=(expected_value_base &&) = default;

  ~expected_value_base() noexcept = default;

  inline constexpr void value() const
  {
    if (!bool(*this))
    {
      GPCL_THROW(bad_expected_access<E>(this->error()));
    }
  }

  inline void emplace()
  {
    if (!*this)
    {
      this->err_.unexpected<E>::~unexpected();
      this->ok_ = true;
    }
  }
};

template <typename T, typename E, typename = void>
class expected_copy_base;

template <typename T, typename E, typename>
class expected_copy_base : public expected_value_base<T, E>
{
public:
  using expected_value_base<T, E>::expected_value_base;

  constexpr expected_copy_base() = default;

  constexpr expected_copy_base(expected_copy_base &&) = default;

  expected_copy_base &operator=(const expected_copy_base &) = default;
  expected_copy_base &operator=(expected_copy_base &&) = default;

  ~expected_copy_base() noexcept = default;

  inline constexpr expected_copy_base(const expected_copy_base &other)
  {
    this->ok_ = other.ok_;
    if (this->ok_)
    {
      ::new (&this->val_) T(other.val_);
    }
    else
    {
      ::new (&this->err_) unexpected<E>(other.err_);
    }
  }
};

template <typename T, typename E>
class expected_copy_base<
    T, E,
    std::enable_if_t<(
        detail::is_trivially_copy_constructible_v<T> ||
        detail::is_void_v<T>)&&detail::is_trivially_copy_constructible_v<E>>>
    : public expected_value_base<T, E>
{
public:
  using expected_value_base<T, E>::expected_value_base;

  constexpr expected_copy_base() = default;

  constexpr expected_copy_base(expected_copy_base &&) = default;
  constexpr expected_copy_base(const expected_copy_base &) = default;

  expected_copy_base &operator=(const expected_copy_base &) = default;
  expected_copy_base &operator=(expected_copy_base &&) = default;

  ~expected_copy_base() noexcept = default;
};

template <typename T, typename E>
class expected_copy_base<
    T, E,
    std::enable_if_t<detail::is_void_v<T> &&
                     (detail::is_copy_constructible_v<E> &&
                      !detail::is_trivially_copy_constructible_v<E>)>>
    : public expected_value_base<T, E>
{
public:
  using expected_value_base<T, E>::expected_value_base;

  constexpr expected_copy_base() = default;

  constexpr expected_copy_base(expected_copy_base &&) = default;

  expected_copy_base &operator=(const expected_copy_base &) = default;
  expected_copy_base &operator=(expected_copy_base &&) = default;

  ~expected_copy_base() noexcept = default;

  inline constexpr expected_copy_base(const expected_copy_base &other)
  {
    this->ok_ = other.ok_;
    if (!this->ok_)
    {
      this->err_ = other.err_;
    }
  }
};

template <typename T, typename E>
class expected_copy_base<
    T, E,
    std::enable_if_t<(!detail::is_void_v<T> &&
                      !detail::is_copy_constructible_v<T>) ||
                     !detail::is_copy_constructible_v<E>>>
    : public expected_value_base<T, E>
{
public:
  using expected_value_base<T, E>::expected_value_base;

  constexpr expected_copy_base() = default;

  constexpr expected_copy_base(expected_copy_base &&) = default;
  constexpr expected_copy_base(const expected_copy_base &) = delete;

  expected_copy_base &operator=(const expected_copy_base &) = default;
  expected_copy_base &operator=(expected_copy_base &&) = default;

  ~expected_copy_base() noexcept = default;
};

template <typename T, typename E, typename = void>
class expected_move_base;

template <typename T, typename E, typename>
class expected_move_base : public expected_copy_base<T, E>
{
public:
  using expected_copy_base<T, E>::expected_copy_base;

  constexpr expected_move_base() = default;

  constexpr expected_move_base(const expected_move_base &) = default;

  expected_move_base &operator=(const expected_move_base &) = default;
  expected_move_base &operator=(expected_move_base &&) = default;

  inline constexpr expected_move_base(expected_move_base &&other) noexcept(
      detail::is_nothrow_move_constructible_v<T>
          &&detail::is_nothrow_move_constructible_v<E>)
  {
    this->ok_ = other.ok_;
    if (this->ok_)
    {
      ::new (&this->val_) T(detail::move(other.val_));
    }
    else
    {
      ::new (&this->err_) unexpected<E>(detail::move(other.err_));
    }
  }
};

template <typename T, typename E>
class expected_move_base<
    T, E,
    std::enable_if_t<(detail::is_void_v<T> ||
                      detail::is_trivially_move_constructible_v<
                          T>)&&detail::is_trivially_move_constructible_v<E>>>
    : public expected_copy_base<T, E>
{
public:
  using expected_copy_base<T, E>::expected_copy_base;

  constexpr expected_move_base() = default;

  constexpr expected_move_base(expected_move_base &&) = default;
  constexpr expected_move_base(const expected_move_base &) = default;

  expected_move_base &operator=(const expected_move_base &) = default;
  expected_move_base &operator=(expected_move_base &&) = default;
};

template <typename T, typename E>
class expected_move_base<
    T, E,
    std::enable_if_t<detail::is_void_v<T> &&
                     detail::is_move_constructible_v<E> &&
                     !detail::is_trivially_move_constructible_v<E>>>
    : public expected_copy_base<T, E>
{
public:
  using expected_copy_base<T, E>::expected_copy_base;

  constexpr expected_move_base() = default;

  constexpr expected_move_base(const expected_move_base &) = default;

  expected_move_base &operator=(const expected_move_base &) = default;
  expected_move_base &operator=(expected_move_base &&) = default;

  inline constexpr expected_move_base(expected_move_base &&other) noexcept(
      detail::is_nothrow_move_constructible_v<T>
          &&detail::is_nothrow_move_constructible_v<E>)
  {
    this->ok_ = other.ok_;
    if (!this->ok_)
    {
      ::new (&this->err_) unexpected<E>(detail::move(other.err_));
    }
  }
};

template <typename T, typename E>
class expected_move_base<
    T, E,
    std::enable_if_t<(!detail::is_void_v<T> &&
                      !detail::is_move_constructible_v<T>) ||
                     !detail::is_move_constructible_v<E>>>
    : public expected_copy_base<T, E>
{
public:
  using expected_copy_base<T, E>::expected_copy_base;

  constexpr expected_move_base() = default;

  constexpr expected_move_base(expected_move_base &&) = default;
  constexpr expected_move_base(const expected_move_base &) = default;

  expected_move_base &operator=(const expected_move_base &) = default;
  expected_move_base &operator=(expected_move_base &&) = default;
};

template <typename T, typename E, typename = void>
class expected_copy_assign_base;

template <typename T, typename E, typename>
class expected_copy_assign_base : public expected_move_base<T, E>
{
public:
  using expected_move_base<T, E>::expected_move_base;

  constexpr expected_copy_assign_base() = default;

  constexpr expected_copy_assign_base(expected_copy_assign_base &&) = default;
  constexpr expected_copy_assign_base(const expected_copy_assign_base &) =
      default;

  expected_copy_assign_base &
  operator=(const expected_copy_assign_base &) = default;
  expected_copy_assign_base &operator=(expected_copy_assign_base &&) = delete;
};

template <typename T, typename E>
class expected_copy_assign_base<
    T, E,
    std::enable_if_t<
        detail::is_void_v<T> && !std::is_trivially_copyable<E>::value &&
        detail::is_copy_constructible_v<E> && detail::is_copy_assignable_v<E>>>
    : public expected_move_base<T, E>
{
public:
  using expected_move_base<T, E>::expected_move_base;

  constexpr expected_copy_assign_base() = default;

  constexpr expected_copy_assign_base(expected_copy_assign_base &&) = default;
  constexpr expected_copy_assign_base(const expected_copy_assign_base &) =
      default;

  expected_copy_assign_base &operator=(expected_copy_assign_base &&) = default;

  inline expected_copy_assign_base &
  operator=(const expected_copy_assign_base &other)
  {
    if (this->ok_ && other.ok_)
    {
      // do nothing
    }
    else if (this->ok_ && !other.ok_)
    {
      ::new (&this->err_) unexpected<E>(other.err_);
      this->ok_ = false;
    }
    else if (!this->ok_ && other.ok_)
    {
      this->err_.unexpected<E>::~unexpected();
      this->ok_ = true;
    }
    else if (!this->ok_ && !other.ok_)
    {
      this->err_ = other.err_;
    }
    return *this;
  }
};

template <typename T, typename E>
class expected_copy_assign_base<
    T, E,
    std::enable_if_t<conjunction_v<
        disjunction<std::is_trivially_copyable<T>, std::is_void<T>>,
        std::is_trivially_copyable<E>>>> : public expected_move_base<T, E>
{
public:
  using expected_move_base<T, E>::expected_move_base;

  constexpr expected_copy_assign_base() = default;

  constexpr expected_copy_assign_base(expected_copy_assign_base &&) = default;
  constexpr expected_copy_assign_base(const expected_copy_assign_base &) =
      default;

  expected_copy_assign_base &
  operator=(const expected_copy_assign_base &) = default;
  expected_copy_assign_base &operator=(expected_copy_assign_base &&) = default;
};

template <typename T, typename E>
class expected_copy_assign_base<
    T, E,
    std::enable_if_t<conjunction_v<
        detail::is_copy_constructible<T>, detail::is_copy_assignable<T>,
        detail::is_copy_constructible<E>, detail::is_copy_assignable<E>,
        negate<conjunction<std::is_trivially_copyable<T>,
                           std::is_trivially_copyable<E>>>,
        disjunction<detail::is_nothrow_move_constructible<E>,
                    detail::is_nothrow_move_constructible<T>>>>>
    : public expected_move_base<T, E>
{
public:
  using expected_move_base<T, E>::expected_move_base;

  constexpr expected_copy_assign_base() = default;

  constexpr expected_copy_assign_base(expected_copy_assign_base &&) = default;
  constexpr expected_copy_assign_base(const expected_copy_assign_base &) =
      default;

  expected_copy_assign_base &operator=(expected_copy_assign_base &&) = default;

  inline expected_copy_assign_base &
  operator=(const expected_copy_assign_base &other)
  {
    if (this->ok_ && other.ok_)
    {
      this->val_ = other.val_;
    }
    else if (this->ok_ && !other.ok_)
    {
      GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_copy_constructible_v<E>)
      {
        this->val_.T::~T();
        ::new (&this->err_) unexpected<E>(other.err_);
        this->ok_ = false;
      }
      else GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_move_constructible_v<E>)
      {
        unexpected<E> tmp = other.err_;
        this->val_.T::~T();
        ::new (&this->err_) unexpected<E>(detail::move(tmp));
        this->ok_ = false;
      }
      else
      {
        T tmp = this->val_;
        this->val_.T::~T();
        GPCL_TRY { ::new (&this->err_) unexpected<E>(other.err_); }
        GPCL_CATCH(...)
        {
          static_assert(detail::is_nothrow_move_constructible_v<T>, "");
          ::new (&this->val_) T(tmp);
          GPCL_RETHROW;
        }
        GPCL_CATCH_END
        this->ok_ = false;
      }
    }
    else if (!this->ok_ && other.ok_)
    {
      GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_copy_constructible_v<T>)
      {
        this->err_.unexpected<E>::~unexpected();
        ::new (&this->val_) T(other.val_);
        this->ok_ = true;
      }
      else GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_move_constructible_v<T>)
      {
        T tmp = other.val_; // can throw!
        this->err_.unexpected<E>::~unexpected();
        ::new (&this->val_) T(detail::move(tmp));
        this->ok_ = true;
      }
      else
      {
        unexpected<E> tmp = this->err_; // can throw!
        this->err_.unexpected<E>::~unexpected();
        GPCL_TRY { ::new (&this->val_) T(other.val_); }
        GPCL_CATCH(...)
        {
          static_assert(detail::is_nothrow_move_constructible_v<E>, "");
          ::new (&this->err_) unexpected<E>(detail::move(tmp));
          GPCL_RETHROW;
        }
        GPCL_CATCH_END
        this->ok_ = true;
      }
    }
    else if (!this->ok_ && !other.ok_)
    {
      this->err_ = other.err_;
    }
    return *this;
  }
};

template <typename T, typename E, typename = void>
class expected_move_assign_base;

template <typename T, typename E, typename>
class expected_move_assign_base : public expected_copy_assign_base<T, E>
{
public:
  using expected_copy_assign_base<T, E>::expected_copy_assign_base;

  constexpr expected_move_assign_base() = default;

  constexpr expected_move_assign_base(expected_move_assign_base &&) = default;
  constexpr expected_move_assign_base(const expected_move_assign_base &) =
      default;

  expected_move_assign_base &
  operator=(const expected_move_assign_base &) = default;
  expected_move_assign_base &operator=(expected_move_assign_base &&) = delete;
};

template <typename T, typename E>
class expected_move_assign_base<
    T, E,
    std::enable_if_t<detail::is_void_v<T> &&
                     detail::is_nothrow_move_constructible_v<E> &&
                     detail::is_nothrow_move_assignable_v<E>>>
    : public expected_copy_assign_base<T, E>
{
public:
  using expected_copy_assign_base<T, E>::expected_copy_assign_base;

  constexpr expected_move_assign_base() = default;

  constexpr expected_move_assign_base(expected_move_assign_base &&) = default;
  constexpr expected_move_assign_base(const expected_move_assign_base &) =
      default;

  expected_move_assign_base &
  operator=(const expected_move_assign_base &) = default;

  expected_move_assign_base &operator=(expected_move_assign_base &&other)
  {
    if (this->ok_ && other.ok_)
    {
      // do nothing
    }
    else if (this->ok_ && !other.ok_)
    {
      ::new (&this->err_) unexpected<E>(detail::move(other.err_));
      this->ok_ = false;
    }
    else if (!this->ok_ && other.ok_)
    {
      this->err_.unexpected<E>::~unexpected();

      this->ok_ = true;
    }
    else if (!this->ok_ && !other.ok_)
    {
      this->err_ = detail::move(other.err_);
    }
    return *this;
  }
};

template <typename T, typename E>
class expected_move_assign_base<
    T, E,
    std::enable_if_t<conjunction_v<std::is_trivially_copyable<T>,
                                   std::is_trivially_copyable<E>>>>
    : public expected_copy_assign_base<T, E>
{
public:
  using expected_copy_assign_base<T, E>::expected_copy_assign_base;

  constexpr expected_move_assign_base() = default;

  constexpr expected_move_assign_base(expected_move_assign_base &&) = default;
  constexpr expected_move_assign_base(const expected_move_assign_base &) =
      default;

  expected_move_assign_base &
  operator=(const expected_move_assign_base &) = default;

  expected_move_assign_base &operator=(expected_move_assign_base &&) = default;
};

template <typename T, typename E>
class expected_move_assign_base<
    T, E,
    std::enable_if_t<conjunction_v<
        negate<conjunction<std::is_trivially_copyable<T>,
                           std::is_trivially_copyable<E>>>,
        detail::is_move_constructible<T>, detail::is_move_assignable<T>,
        detail::is_nothrow_move_constructible<E>,
        detail::is_nothrow_move_assignable<E>>>>
    : public expected_copy_assign_base<T, E>
{
public:
  using expected_copy_assign_base<T, E>::expected_copy_assign_base;

  constexpr expected_move_assign_base() = default;

  constexpr expected_move_assign_base(expected_move_assign_base &&) = default;
  constexpr expected_move_assign_base(const expected_move_assign_base &) =
      default;

  expected_move_assign_base &
  operator=(const expected_move_assign_base &) = default;

  expected_move_assign_base &
  operator=(expected_move_assign_base &&other) noexcept(
      detail::is_nothrow_move_assignable_v<T>
          &&detail::is_nothrow_move_constructible_v<T>)
  {
    if (this->ok_ && other.ok_)
    {
      this->val_ = detail::move(other.val_);
    }
    else if (this->ok_ && !other.ok_)
    {
      GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_move_constructible_v<E>)
      {
        this->val_.T::~T();
        ::new (&this->err_) unexpected<E>(detail::move(other.err_));
        this->ok_ = false;
      }
      else
      {
        T tmp = detail::move(this->val_);
        this->val_.T::~T();
        GPCL_TRY { ::new (&this->err_) unexpected<E>(detail::move(other.err_)); }
        GPCL_CATCH(...)
        {
          static_assert(detail::is_nothrow_move_constructible_v<T>);
          ::new (&this->val_) T(tmp);
          GPCL_RETHROW;
        }
        GPCL_CATCH_END
        this->ok_ = false;
      }
    }
    else if (!this->ok_ && other.ok_)
    {
      GPCL_CXX17_IF_CONSTEXPR(detail::is_nothrow_move_constructible_v<T>)
      {
        this->err_.unexpected<E>::~unexpected();
        ::new (&this->val_) T(detail::move(other.val_));
        this->ok_ = true;
      }
      else
      {
        unexpected<E> tmp = detail::move(this->err_);
        this->err_.unexpected<E>::~unexpected();
        GPCL_TRY { ::new (&this->val_) T(detail::move(other.val_)); }
        GPCL_CATCH(...)
        {
          static_assert(detail::is_nothrow_move_constructible_v<E>);
          ::new (&this->err_) unexpected<E>(detail::move(tmp));
          GPCL_RETHROW;
        }
        GPCL_CATCH_END
        this->ok_ = true;
      }
    }
    else if (!this->ok_ && !other.ok_)
    {
      this->err_ = detail::move(other.err_);
    }
    return *this;
  }
};

template <typename T, typename E, typename U, typename G, typename = void>
struct expected_convert_constructible
    : std::integral_constant<
          bool,
          detail::is_constructible_v<T, const U &> &&
              !detail::is_constructible_v<T, expected<U, G> &> &&
              !detail::is_constructible_v<T, expected<U, G> &&> &&
              !detail::is_constructible_v<T, const expected<U, G> &> &&
              !detail::is_constructible_v<T, const expected<U, G> &&> &&
              !detail::is_convertible_v<expected<U, G> &, T> &&
              !detail::is_convertible_v<expected<U, G> &&, T> &&
              !detail::is_convertible_v<const expected<U, G> &, T> &&
              !detail::is_convertible_v<const expected<U, G> &&, T> &&
              !detail::is_constructible_v<E, const G &> &&
              !detail::is_constructible_v<unexpected<E>, expected<U, G> &> &&
              !detail::is_constructible_v<unexpected<E>, expected<U, G> &&> &&
              !detail::is_constructible_v<unexpected<E>,
                                          const expected<U, G> &> &&
              !detail::is_constructible_v<unexpected<E>,
                                          const expected<U, G> &&> &&
              !detail::is_convertible_v<expected<U, G> &, unexpected<E>> &&
              !detail::is_convertible_v<expected<U, G> &&, unexpected<E>> &&
              !detail::is_convertible_v<const expected<U, G> &,
                                        unexpected<E>> &&
              !detail::is_convertible_v<const expected<U, G> &&, unexpected<E>>>
{
};

template <typename T, typename E, typename U, typename G>
struct expected_convert_constructible<
    T, E, U, G, std::enable_if_t<detail::is_void_v<T> || detail::is_void_v<G>>>
    : std::false_type
{
};

template <typename T, typename E, typename U, typename G>
constexpr bool expected_convert_constructible_v =
    expected_convert_constructible<T, E, U, G>();

#endif

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_EXPECTED_HPP
