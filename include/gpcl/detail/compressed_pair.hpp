//
// compressed_pair.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_COMPRESSED_PAIR_HPP
#define GPCL_DETAIL_COMPRESSED_PAIR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/swap.hpp>

#include <tuple>
#include <type_traits>

namespace gpcl {
namespace detail {

template <typename T, std::size_t I, bool IsEmpty = std::is_empty<T>::value>
class compressed_storage;

template <typename T, std::size_t I>
class compressed_storage<T, I, false>
{
  T data_;

public:
  using type = T;

  template <typename... Args>
  explicit constexpr compressed_storage(Args &&...args)
      : data_(std::forward<Args>(args)...)
  {
  }

  void swap(compressed_storage &other) noexcept(std::is_nothrow_swappable_v<T>)
  {
    using gpcl::swap;
    swap(data_, other.data_);
  }

  T &get() &noexcept { return data_; }
  T &&get() &&noexcept { return std::move(data_); };

  T const &get() const &noexcept { return data_; }
  T const &&get() const &&noexcept { return data_; }
};

template <typename T, std::size_t I>
class compressed_storage<T, I, true> : private T
{
public:
  using type = T;

  template <typename... Args>
  explicit constexpr compressed_storage(Args &&...args)
      : T(std::forward<Args>(args)...)
  {
  }

  void swap(compressed_storage &other) noexcept(std::is_nothrow_swappable_v<T>)
  {
    using gpcl::swap;
    swap(static_cast<T &>(*this), static_cast<T &>(other));
  }

  T &get() &noexcept { return *this; }
  T &&get() &&noexcept { return std::move(*this); }

  T const &get() const &noexcept { return *this; }
  T const &&get() const &&noexcept { return *this; }
};

enum piecewise_construct_t
{
  piecewise_construct
};

template <typename T1, typename T2>
class compressed_pair : private compressed_storage<T1, 1>,
                        private compressed_storage<T2, 2>
{
  using base_type_1 = compressed_storage<T1, 1>;
  using base_type_2 = compressed_storage<T2, 2>;

  template <typename Tuple1, typename Tuple2, std::size_t... Is,
            std::size_t... Js>
  constexpr compressed_pair(Tuple1 &&tp1, Tuple2 &&tp2,
                            std::index_sequence<Is...>,
                            std::index_sequence<Js...>)
      : base_type_1(std::get<Is>(std::forward<Tuple1>(tp1))...),
        base_type_2(std::get<Js>(std::forward<Tuple2>(tp2))...)
  {
  }

public:
  using first_type = T1;
  using second_type = T2;

  constexpr compressed_pair() = default;

  template <typename U1>
  explicit constexpr compressed_pair(U1 &&x) : base_type_1(std::forward<U1>(x))
  {
  }

  template <typename U1, typename U2>
  constexpr compressed_pair(U1 &&x, U2 &&y)
      : base_type_1(std::forward<U1>(x)),
        base_type_2(std::forward<U2>(y))
  {
  }

  template <typename... Xs, typename... Ys>
  constexpr compressed_pair(piecewise_construct_t, std::tuple<Xs...> tp1,
                            std::tuple<Ys...> tp2)
      : compressed_pair(std::move(tp1), std::move(tp2),
                        std::index_sequence_for<Xs...>(),
                        std::index_sequence_for<Ys...>())
  {
  }

  constexpr compressed_pair(const compressed_pair &) = default;
  constexpr compressed_pair(compressed_pair &&) noexcept(
      std::is_nothrow_move_constructible_v<T1>
          &&std::is_nothrow_move_constructible_v<T2>) = default;

  ~compressed_pair() = default;

  compressed_pair &operator=(const compressed_pair &) = default;
  compressed_pair &operator=(compressed_pair &&) noexcept(
      std::is_nothrow_move_assignable_v<T1>
          &&std::is_nothrow_move_assignable_v<T2>) = default;

  void swap(compressed_pair &other) noexcept(
      std::is_nothrow_swappable_v<T1> &&std::is_nothrow_swappable_v<T2>)
  {
    base_type_1::swap(other);
    base_type_2::swap(other);
  }

  T1 &first() &noexcept { return base_type_1::get(); }

  T1 &&first() &&noexcept { return std::move(*this).base_type_1::get(); }

  T1 const &first() const &noexcept { return base_type_1::get(); }

  T1 const &&first() const &&noexcept
  {
    return std::move(*this).base_type_1::get();
  }

  T2 &second() &noexcept { return base_type_2::get(); }

  T2 &&second() &&noexcept { return std::move(*this).base_type_2::get(); }

  T2 const &second() const &noexcept { return base_type_2::get(); }

  T2 const &&second() const &&noexcept
  {
    return std::move(*this).base_type_2::get();
  }
};

template <typename T1, typename T2>
inline void swap(compressed_pair<T1, T2> &x,
                 compressed_pair<T1, T2> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_COMPRESSED_PAIR_HPP
