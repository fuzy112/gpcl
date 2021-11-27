//
// propagate_const.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PROPAGATE_CONST_HPP
#define GPCL_PROPAGATE_CONST_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {

template <typename T>
class propagate_const;

template <typename T>
struct is_propagate_const : std::false_type
{
};
template <typename T>
struct is_propagate_const<propagate_const<T>> : std::true_type
{
};

template <typename T>
class propagate_const
{
  T t_;

public:
  using element_type = std::remove_reference_t<decltype(*std::declval<T &>())>;

  constexpr propagate_const() = default;
  constexpr propagate_const(propagate_const &&) = default;

  template <typename U, std::enable_if_t<std::is_constructible_v<T, U> &&
                                             std::is_convertible_v<U, T>,
                                         int> = 0>
  propagate_const(propagate_const<U> &&pu) : t_(std::move(pu.t_))
  {
  }

  template <typename U, std::enable_if_t<std::is_constructible_v<T, U> &&
                                             !std::is_convertible_v<U, T>,
                                         int> = 0>
  explicit propagate_const(propagate_const<U> &&pu) : t_(std::move(pu.t_))
  {
  }

  template <typename U,
            std::enable_if_t<std::is_constructible_v<T, U> &&
                                 !is_propagate_const<std::decay_t<U>>::value &&
                                 std::is_convertible_v<U, T>,
                             int> = 0>
  propagate_const(U &&u) : t_(std::forward<U>(u))
  {
  }

  template <typename U,
            std::enable_if_t<std::is_constructible_v<T, U> &&
                                 !is_propagate_const<std::decay_t<U>>::value &&
                                 !std::is_convertible_v<U, T>,
                             int> = 0>
  explicit propagate_const(U &&u) : t_(std::forward<U>(u))
  {
  }

  propagate_const(const propagate_const &) = delete;

  constexpr propagate_const &operator=(propagate_const &&p) = default;

  template <typename U, std::enable_if_t<std::is_constructible_v<T, U> &&
                                             std::is_convertible_v<U, T>,
                                         int> = 0>
  constexpr propagate_const &operator=(propagate_const<U> &&pu)
  {
    t_ = std::move(pu.t_);
    return *this;
  }

  template <typename U,
            std::enable_if_t<std::is_constructible_v<T, U> &&
                                 !is_propagate_const<std::decay_t<U>>::value &&
                                 std::is_convertible_v<U, T>,
                             int> = 0>
  constexpr propagate_const &operator=(U &&u)
  {
    t_ = std::forward<U>(u);
    return *this;
  }

  propagate_const &operator=(const propagate_const &) = delete;

  constexpr void
  swap(propagate_const &other) noexcept(std::is_nothrow_swappable_v<T>)
  {
    using std::swap;
    swap(t_, other.t_);
  }

  constexpr element_type *get()
  {
    if (!t_)
      return nullptr;
    return std::addressof(*t_);
  }

  constexpr const element_type *get() const
  {
    if (!t_)
      return nullptr;
    return std::addressof(*t_);
  }

  constexpr explicit operator bool() const { return bool(t_); }

  constexpr element_type &operator*() { return *t_; }

  constexpr const element_type &operator*() const { return *t_; }

  constexpr element_type *operator->()
  {
    GPCL_ASSERT(*this);
    return get();
  }

  constexpr const element_type *operator->() const
  {
    GPCL_ASSERT(*this);
    return get();
  }

  constexpr operator element_type *() { return get(); }

  constexpr operator const element_type *() const { return get(); }
};

template <typename T>
constexpr bool operator==(const propagate_const<T> &pt, std::nullptr_t)
{
  return pt.get() == nullptr;
}

template <typename T>
constexpr bool operator==(std::nullptr_t, const propagate_const<T> &pt)
{
  return pt.get() == nullptr;
}

template <typename T>
constexpr bool operator!=(const propagate_const<T> &pt, std::nullptr_t)
{
  return pt.get() != nullptr;
}

template <typename T>
constexpr bool operator!=(std::nullptr_t, const propagate_const<T> &pt)
{
  return pt.get() != nullptr;
}

template <typename T, typename U>
constexpr bool operator==(const propagate_const<T> &pt,
                          const propagate_const<U> &pu)
{
  return pt.get() == pu.get();
}

template <typename T, typename U>
constexpr bool operator!=(const propagate_const<T> &pt,
                          const propagate_const<U> &pu)
{
  return pt.get() != pu.get();
}

template <typename T, typename U>
constexpr bool operator<(const propagate_const<T> &pt,
                         const propagate_const<U> &pu)
{
  return pt.get() < pu.get();
}

template <typename T, typename U>
constexpr bool operator>(const propagate_const<T> &pt,
                         const propagate_const<U> &pu)
{
  return pt.get() > pu.get();
}

template <typename T, typename U>
constexpr bool operator<=(const propagate_const<T> &pt,
                          const propagate_const<U> &pu)
{
  return pt.get() <= pu.get();
}

template <typename T, typename U>
constexpr bool operator>=(const propagate_const<T> &pt,
                          const propagate_const<U> &pu)
{
  return pt.get() >= pu.get();
}

template <typename T, typename U>
constexpr bool operator==(const propagate_const<T> &pt, const U &u)
{
  return pt.get() == u;
}

template <typename T, typename U>
constexpr bool operator!=(const propagate_const<T> &pt, const U &u)
{
  return pt.get() != u;
}

template <typename T, typename U>
constexpr bool operator==(const T &t, const propagate_const<U> &pu)
{
  return t == pu.get();
}

template <typename T, typename U>
constexpr bool operator!=(const T &t, const propagate_const<U> &pu)
{
  return t != pu.get();
}

template <typename T, typename U>
constexpr bool operator<(const propagate_const<T> &pt, const U &u)
{
  return pt.get() < u;
}

template <typename T, typename U>
constexpr bool operator>(const propagate_const<T> &pt, const U &u)
{
  return pt.get() > u;
}

template <typename T, typename U>
constexpr bool operator<=(const propagate_const<T> &pt, const U &u)
{
  return pt.get() <= u;
}

template <typename T, typename U>
constexpr bool operator>=(const propagate_const<T> &pt, const U &u)
{
  return pt.get() >= u;
}

template <typename T, typename U>
constexpr bool operator<(const T &t, const propagate_const<U> &pu)
{
  return t < pu.get();
}

template <typename T, typename U>
constexpr bool operator>(const T &t, const propagate_const<U> &pu)
{
  return t > pu.get();
}

template <typename T, typename U>
constexpr bool operator<=(const T &t, const propagate_const<U> &pu)
{
  return t <= pu.get();
}

template <typename T, typename U>
constexpr bool operator>=(const T &t, const propagate_const<U> &pu)
{
  return t >= pu.get();
}

template <typename T, std::enable_if_t<std::is_swappable_v<T>, int> = 0>
void swap(propagate_const<T> &x,
          propagate_const<T> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

} // namespace gpcl

#endif // GPCL_PROPAGATE_CONST_HPP
