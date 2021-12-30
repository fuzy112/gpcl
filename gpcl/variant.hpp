//
// variant.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_VARIANT_HPP
#define GPCL_VARIANT_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/swap.hpp>

#include <cstddef>
#include <exception>
#include <functional>
#include <utility>

namespace gpcl {

class bad_variant_access : public std::exception
{
public:
  bad_variant_access() = default;
  bad_variant_access(const bad_variant_access &) noexcept = default;

  const char *what() const noexcept final { return "bad variant access"; }
};

constexpr std::size_t variant_npos = -1;

template <typename... Types>
class variant;

template <typename Variant>
struct variant_size;

template <typename... Types>
struct variant_size<variant<Types...>>
    : std::integral_constant<std::size_t, sizeof...(Types)>
{
};

template <typename T>
struct variant_size<T const> : variant_size<T>
{
};

template <typename T>
struct variant_size<T volatile> : variant_size<T>
{
};

template <typename T>
struct variant_size<T const volatile> : variant_size<T>
{
};

template <typename T>
struct variant_size<T &> : variant_size<T>
{
};

template <typename T>
struct variant_size<T &&> : variant_size<T>
{
};

template <typename Variant>
constexpr std::size_t variant_size_v = variant_size<Variant>::value;

template <std::size_t I, typename Variant>
struct variant_alternative;

template <typename T, typename... Types>
struct variant_alternative<0, variant<T, Types...>>
{
  using type = T;
};

template <std::size_t I, typename T, typename... Types>
struct variant_alternative<I, variant<T, Types...>>
    : variant_alternative<I - 1, variant<Types...>>
{
};


template <std::size_t I, typename T>
struct variant_alternative<I, T const> : variant_alternative<I, T>
{
};

template <std::size_t I, typename T>
struct variant_alternative<I, T volatile> : variant_alternative<I, T>
{
};

template <std::size_t I, typename T>
struct variant_alternative<I, T const volatile> : variant_alternative<I, T>
{
};

template <std::size_t I, typename T>
struct variant_alternative<I, T &> : variant_alternative<I, T>
{
};

template <std::size_t I, typename T>
struct variant_alternative<I, T &&> : variant_alternative<I, T>
{
};

template <std::size_t I, typename Variant>
using variant_alternative_t = typename variant_alternative<I, Variant>::type;

namespace detail {

template <std::size_t I, typename T, typename... Types>
struct find_type_index_impl;

template <std::size_t I, typename T, typename... Types>
struct find_type_index_impl<I, T, T, Types...>
    : std::integral_constant<std::size_t, I>
{
};

template <std::size_t I, typename T>
struct find_type_index_impl<I, T>
{
};

template <std::size_t I, typename T, typename U, typename... Types>
struct find_type_index_impl<I, T, U, Types...>
    : find_type_index_impl<I + 1, T, Types...>
{
};

template <typename T, typename... Types>
struct find_type_index : find_type_index_impl<0, T, Types...>
{
};

// clang-format off
template <std::size_t MaxIndex>
using variant_index_type_impl = 
    std::conditional_t<MaxIndex < (std::size_t)(std::numeric_limits<signed char>::max)(), signed char,
    std::conditional_t<MaxIndex < (std::size_t)(std::numeric_limits<signed short>::max)(), signed short,
    std::conditional_t<MaxIndex < (std::size_t)(std::numeric_limits<signed int>::max)(), signed int, 
    std::size_t>>>;
// clang-format on

template <typename... Types>
using variant_index_type = variant_index_type_impl<sizeof...(Types)>;

// ----------------------- Destructor -----------------------------

template <bool AllTriviallyDestructible, typename... Types>
class variant_storage;

template <typename... Types>
class variant_storage<true, Types...>
{
public:
  union
  {
    char dummy_;
    std::aligned_union_t<0, char, Types...> storage_;
  };

  variant_index_type<Types...> index_;

  constexpr variant_storage() : dummy_(), index_(-1) {}

  constexpr void reset() noexcept { index_ = -1; }
};

template <typename... Types>
class variant_storage<false, Types...>
{
public:
  union
  {
    char dummy_;
    std::aligned_union_t<0, char, Types...> storage_;
  };
  variant_index_type<Types...> index_;

  constexpr variant_storage() : dummy_(), index_(-1) {}

  constexpr variant_storage(const variant_storage &) = default;
  constexpr variant_storage(variant_storage &&) = default;
  constexpr variant_storage &operator=(const variant_storage &) = default;
  constexpr variant_storage &operator=(variant_storage &&) = default;

  ~variant_storage()
  {
    if (index_ != -1)
    {
      destruct_helper(index_, &storage_, std::index_sequence_for<Types...>{});
    }
  }

  constexpr void reset() noexcept
  {
    if (index_ != -1)
    {
      destruct_helper(index_, &storage_, std::index_sequence_for<Types...>{});
      index_ = -1;
    }
  }

private:
  template <bool False = false>
  static constexpr void destruct_helper(std::size_t /*index*/,
                                        void * /*object*/,
                                        std::index_sequence<>) noexcept
  {
    GPCL_UNREACHABLE("invalid state");
  }

  template <std::size_t I, std::size_t... Is>
  static constexpr void destruct_helper(std::size_t index, void *object,
                                        std::index_sequence<I, Is...>) noexcept
  {
    if (index == I)
    {
      using type = variant_alternative_t<I, variant<Types...>>;
      static_cast<type *>(object)->~type();
    }
    else
    {
      destruct_helper(index, object, std::index_sequence<Is...>{});
    }
  }
};

template <typename T, bool J, typename... Types>
constexpr T *unsafe_get(variant_storage<J, Types...> *var) noexcept
{
  return reinterpret_cast<T *>(&var->storage_);
}

template <typename T, bool J, typename... Types>
constexpr const T *unsafe_get(variant_storage<J, Types...> const *var) noexcept
{
  return reinterpret_cast<T const *>(&var->storage_);
}

//  --------------------- Move constructor ------------------------------

template <bool AllTriviallyMoveConstructible, typename... Types>
class variant_move_construct_base;

template <typename... Types>
class variant_move_construct_base<false, Types...>
    : public variant_storage<(std::is_trivially_destructible_v<Types> && ...),
                             Types...>
{
  static constexpr bool nothrow =
      (std::is_nothrow_move_constructible_v<Types> && ...);

public:
  constexpr variant_move_construct_base() = default;

  template <bool True = true,
            std::enable_if_t<
                True && (std::is_move_constructible_v<Types> && ...), int> = 0>
  constexpr variant_move_construct_base(
      variant_move_construct_base &&other) noexcept(nothrow)
  {
    if (other.index_ == -1)
    {
      this->index_ = -1;
      return;
    }
    else
    {
      move_helper(other, std::index_sequence_for<Types...>{});
    }
  }

  constexpr variant_move_construct_base(const variant_move_construct_base &) =
      default;

  constexpr variant_move_construct_base &
  operator=(const variant_move_construct_base &) = default;

  constexpr variant_move_construct_base &
  operator=(variant_move_construct_base &&) = default;

protected:
  constexpr void move_helper(variant_move_construct_base &,
                             std::index_sequence<>) noexcept
  {
  }

  template <std::size_t I, std::size_t... Is>
  constexpr void move_helper(variant_move_construct_base &other,
                             std::index_sequence<I, Is...>) noexcept(nothrow)
  {
    if (other.index_ == I)
    {
      using type = variant_alternative_t<I, variant<Types...>>;
      new (unsafe_get<type>(*this)) type{std::move(*(unsafe_get<type>(other)))};
      this->index_ = I;
      return;
    }
    else
    {
      move_helper(other, std::index_sequence<Is...>{});
    }
  }
};

template <typename... Types>
class variant_move_construct_base<true, Types...>
    : public variant_storage<(std::is_trivially_destructible_v<Types> && ...),
                             Types...>
{
  static constexpr bool nothrow =
      (std::is_nothrow_move_constructible_v<Types> && ...);

public:
  constexpr variant_move_construct_base() = default;

  constexpr variant_move_construct_base(variant_move_construct_base &&other) =
      default;

  constexpr variant_move_construct_base(const variant_move_construct_base &) =
      default;

  constexpr variant_move_construct_base &
  operator=(const variant_move_construct_base &) = default;

  constexpr variant_move_construct_base &
  operator=(variant_move_construct_base &&) = default;
};

// ---------------- Move constructor ----------------------

template <bool AllTriviallyCopyConstructible, typename... Types>
class variant_copy_construct_base;

template <typename... Types>
class variant_copy_construct_base<false, Types...>
    : public variant_move_construct_base<
          (std::is_trivially_move_constructible_v<Types> && ...), Types...>
{
public:
  constexpr variant_copy_construct_base() = default;

  constexpr variant_copy_construct_base(variant_copy_construct_base &&other) =
      default;

  template <bool True = true,
            std::enable_if_t<
                True && (std::is_copy_constructible_v<Types> && ...), int> = 0>
  constexpr variant_copy_construct_base(
      const variant_copy_construct_base &other)
  {
    if (other.index_ == -1)
    {
      this->index_ = -1;
      return;
    }
    else
    {
      copy_helper(other, std::index_sequence_for<Types...>{});
    }
  }

  constexpr variant_copy_construct_base &
  operator=(const variant_copy_construct_base &) = default;

  constexpr variant_copy_construct_base &
  operator=(variant_copy_construct_base &&) = default;

protected:
  constexpr void copy_helper(variant_copy_construct_base &,
                             std::index_sequence<>) noexcept
  {
  }

  template <std::size_t I, std::size_t... Is>
  constexpr void copy_helper(variant_copy_construct_base &other,
                             std::index_sequence<I, Is...>)
  {
    if (other.index_ == I)
    {
      using type = variant_alternative_t<I, variant<Types...>>;
      new (unsafe_get<type>(*this)) type(*(unsafe_get<type>(other)));
      this->index_ = I;
      return;
    }
    else
    {
      copy_helper(other, std::index_sequence<Is...>{});
    }
  }
};

template <typename... Types>
class variant_copy_construct_base<true, Types...>
    : public variant_move_construct_base<
          (std::is_trivially_move_constructible_v<Types> && ...), Types...>
{
public:
  constexpr variant_copy_construct_base() = default;

  constexpr variant_copy_construct_base(variant_copy_construct_base &&other) =
      default;

  constexpr variant_copy_construct_base(
      const variant_copy_construct_base &other) = default;

  constexpr variant_copy_construct_base &
  operator=(const variant_copy_construct_base &) = default;

  constexpr variant_copy_construct_base &
  operator=(variant_copy_construct_base &&) = default;
};

// ---------------- Move assignment -------------------------------------

template <bool Movable, bool Trivial, typename... Types>
class variant_move_assign_base;

template <typename... Types>
class variant_move_assign_base<false, false, Types...>
    : public variant_copy_construct_base<
          (std::is_trivially_copy_constructible_v<Types> && ...), Types...>
{
public:
  constexpr variant_move_assign_base() = default;

  constexpr variant_move_assign_base(variant_move_assign_base &&other) =
      default;

  constexpr variant_move_assign_base(const variant_move_assign_base &other) =
      default;

  constexpr variant_move_assign_base &
  operator=(const variant_move_assign_base &) = default;

  constexpr variant_move_assign_base &
  operator=(variant_move_assign_base &&other) = delete;
};

template <typename... Types>
class variant_move_assign_base<true, true, Types...>
    : public variant_copy_construct_base<
          (std::is_trivially_copy_constructible_v<Types> && ...), Types...>
{
public:
  constexpr variant_move_assign_base() = default;

  constexpr variant_move_assign_base(variant_move_assign_base &&other) =
      default;

  constexpr variant_move_assign_base(const variant_move_assign_base &other) =
      default;

  constexpr variant_move_assign_base &
  operator=(const variant_move_assign_base &) = default;

  constexpr variant_move_assign_base &
  operator=(variant_move_assign_base &&other) = default;
};

template <typename... Types>
class variant_move_assign_base<true, false, Types...>
    : public variant_copy_construct_base<
          (std::is_trivially_copy_constructible_v<Types> && ...), Types...>
{
public:
  constexpr variant_move_assign_base() = default;

  constexpr variant_move_assign_base(variant_move_assign_base &&other) =
      default;

  constexpr variant_move_assign_base(const variant_move_assign_base &other) =
      default;

  constexpr variant_move_assign_base &
  operator=(const variant_move_assign_base &) = default;

  constexpr variant_move_assign_base &
  operator=(variant_move_assign_base &&other) noexcept(
      ((std::is_nothrow_move_constructible_v<Types> &&
        std::is_nothrow_move_assignable_v<Types>)&&...))
  {
    if (this == &other)
      return *this;

    if (this->index_ == other._index && this->index_ != -1)
    {
      move_assign_helper(other, std::index_sequence_for<Types...>{});
    }
    else
    {
      this->reset();

      if constexpr ((std::is_trivially_move_constructible_v<Types> && ...))
        std::memmove(this, &other, sizeof(other));
      else
        this->move_helper(other, std::index_sequence_for<Types...>{});
    }

    return *this;
  }

protected:
  constexpr void move_assign_helper(variant_move_assign_base &,
                                    std::index_sequence<>) noexcept
  {
  }

  template <std::size_t I, std::size_t... Is>
  constexpr void move_assign_helper(variant_move_assign_base &other,
                                    std::index_sequence<I, Is...>)
  {
    if (other.index_ == I)
    {
      using type = variant_alternative_t<I, variant<Types...>>;
      *(unsafe_get<type>(*this)) = std::move(*(unsafe_get<type>(other)));
    }
    else
    {
      move_assign_helper(other, std::index_sequence<Is...>{});
    }
  }
};

// ------------------- Copy asignment ---------------------------------

template <bool Movable, bool Trivial, typename... Types>
class variant_copy_assign_base;

template <typename... Types>
class variant_copy_assign_base<false, false, Types...>
    : public variant_move_assign_base<
          ((std::is_move_constructible_v<Types> &&
            std::is_move_assignable_v<Types>)&&...),
          ((std::is_trivially_move_constructible_v<Types> &&
            std::is_trivially_move_assignable_v<Types>)&&...),
          Types...>
{
public:
  constexpr variant_copy_assign_base() = default;

  constexpr variant_copy_assign_base(variant_copy_assign_base &&other) =
      default;

  constexpr variant_copy_assign_base(const variant_copy_assign_base &other) =
      default;

  constexpr variant_copy_assign_base &
  operator=(const variant_copy_assign_base &) = delete;

  constexpr variant_copy_assign_base &
  operator=(variant_copy_assign_base &&other) = default;
};

template <typename... Types>
class variant_copy_assign_base<true, true, Types...>
    : public variant_move_assign_base<
          ((std::is_move_constructible_v<Types> &&
            std::is_move_assignable_v<Types>)&&...),
          ((std::is_trivially_move_constructible_v<Types> &&
            std::is_trivially_move_assignable_v<Types>)&&...),
          Types...>
{
public:
  constexpr variant_copy_assign_base() = default;

  constexpr variant_copy_assign_base(variant_copy_assign_base &&other) =
      default;

  constexpr variant_copy_assign_base(const variant_copy_assign_base &other) =
      default;

  constexpr variant_copy_assign_base &
  operator=(const variant_copy_assign_base &) = default;

  constexpr variant_copy_assign_base &
  operator=(variant_copy_assign_base &&other) = default;
};

template <typename... Types>
class variant_copy_assign_base<true, false, Types...>
    : public variant_move_assign_base<
          ((std::is_move_constructible_v<Types> &&
            std::is_move_assignable_v<Types>)&&...),
          ((std::is_trivially_move_constructible_v<Types> &&
            std::is_trivially_move_assignable_v<Types>)&&...),
          Types...>
{
public:
  constexpr variant_copy_assign_base() = default;

  constexpr variant_copy_assign_base(variant_copy_assign_base &&other) =
      default;

  constexpr variant_copy_assign_base(const variant_copy_assign_base &other) =
      default;

  constexpr variant_copy_assign_base &
  operator=(const variant_copy_assign_base &) = default;

  constexpr variant_copy_assign_base &
  operator=(variant_copy_assign_base &&other)
  {
    if (this == &other)
      return *this;

    if (this->index_ == other.index_ && this->index_ != -1)
    {
      copy_assign_helper(other, std::index_sequence_for<Types...>{});
    }
    else
    {
      this->reset();

      if constexpr ((std::is_trivially_copy_constructible_v<Types> && ...))
        std::memcpy(this, &other, sizeof(other));
      else
        this->copy_helper(other, std::index_sequence_for<Types...>{});
    }

    return *this;
  }

protected:
  constexpr void copy_assign_helper(variant_copy_assign_base &,
                                    std::index_sequence<>) noexcept
  {
  }

  template <std::size_t I, std::size_t... Is>
  constexpr void copy_assign_helper(variant_copy_assign_base &other,
                                    std::index_sequence<I, Is...>)
  {
    if (other.index_ == I)
    {
      using type = variant_alternative_t<I, variant<Types...>>;
      *(unsafe_get<type>(*this)) = *(unsafe_get<type>(other));
    }
    else
    {
      copy_assign_helper(other, std::index_sequence<Is...>{});
    }
  }
};

// --------------- Default constructor --------------------------------

struct variant_noinit_tag
{
};

template <bool FirstDefaultConstructible, typename... Types>
class variant_default_construct_base;

template <typename... Types>
class variant_default_construct_base<false, Types...>
    : public variant_copy_assign_base<
          ((std::is_copy_constructible_v<Types> &&
            std::is_copy_assignable_v<Types>)&&...),
          ((std::is_trivially_copy_constructible_v<Types> &&
            std::is_trivially_copy_assignable_v<Types>)&&...),
          Types...>
{
  using base_type = variant_copy_construct_base<
      (std::is_trivially_copy_constructible_v<Types> && ...), Types...>;

public:
  constexpr variant_default_construct_base() = delete;

  constexpr variant_default_construct_base(variant_noinit_tag) noexcept
      : base_type()
  {
  }
};

template <typename... Types>
class variant_default_construct_base<true, Types...>
    : public variant_copy_assign_base<
          ((std::is_copy_constructible_v<Types> &&
            std::is_copy_assignable_v<Types>)&&...),
          ((std::is_trivially_copy_constructible_v<Types> &&
            std::is_trivially_copy_assignable_v<Types>)&&...),
          Types...>
{
  using base_type = variant_copy_assign_base<
      ((std::is_copy_constructible_v<Types> &&
        std::is_copy_assignable_v<Types>)&&...),
      ((std::is_trivially_copy_constructible_v<Types> &&
        std::is_trivially_copy_assignable_v<Types>)&&...),
      Types...>;
  using first_type = variant_alternative_t<0, variant<Types...>>;

public:
  constexpr variant_default_construct_base(variant_noinit_tag) noexcept
      : base_type()
  {
  }

  constexpr variant_default_construct_base() : base_type()
  {
    if constexpr (!std::is_trivially_default_constructible_v<first_type>)
      new (unsafe_get<first_type>(*this)) first_type();
    this->index_ = 0;
  }
};

// ------------------ Convert Constructor --------------------------

template <typename T>
class resolution_helper_impl
{
public:
  T resolute(T x);
};

template <typename... Types>
class resolution_helper : private resolution_helper_impl<Types>...
{
public:
  using resolution_helper_impl<Types>::resolute...;
};

// ---------------- Base alias -----------------------------

template <typename... Types>
using variant_base = variant_default_construct_base<
    std::is_default_constructible_v<
        variant_alternative_t<0, variant<Types...>>>,
    Types...>;

} // namespace detail

using detail::unsafe_get;

// ------------------- Variant ------------------------------

/// @bug This implementation heavily uses placement new, but placement new
/// cannot be used in constant expression until C++ 20, so most of the constexpr
/// functions are not truly constexpr.
template <typename... Types>
class variant : public detail::variant_base<Types...>
{
  using base_type = detail::variant_base<Types...>;

  constexpr void set_index(std::size_t i) noexcept
  {
    GPCL_ASSERT_CONST(base_type::index_ == -1);
    base_type::index_ = i;
  }

public:
  /// @name Constructors
  /// @{
  template <typename T0 = variant_alternative_t<0, variant>,
            std::enable_if_t<std::is_default_constructible_v<T0>, int> = 0>
  constexpr variant() noexcept(std::is_nothrow_default_constructible_v<T0>)
      : base_type()
  {
  }

#if defined GPCL_DOXYGEN

  constexpr variant(variant &&other) noexcept(...);

  constexpr variant(variant const &other);

  template <typename T>
  constexpr variant(T &&t) noexcept(...);

#else

  using base_type::base_type;

  template <class T, class U = decltype(detail::resolution_helper<Types...>()
                                            .resolute(std::declval<T>()))>
  constexpr variant(T &&t) noexcept(std::is_nothrow_constructible_v<U, T &&>)
      : base_type(detail::variant_noinit_tag{})
  {
    if constexpr (std::is_trivial_v<U>)
      *static_cast<T *>(unsafe_get<U>(this)) = t;
    else
      new (unsafe_get<U>(this)) U(std::forward<T>(t));
    set_index(detail::find_type_index<U, Types...>());
  }
#endif
  /// @}

  /// @name Assignment operators
  /// @{
#if defined GPCL_DOXYGEN
  constexpr variant &operator=(const variant &other);

  constexpr variant &operator=(variant &&other) noexcept(...);

  template <typename T>
  constexpr variant &operator=(T &&) noexcept(...);

#else
  using base_type::operator=;

  template <class T, class U = decltype(detail::resolution_helper<Types...>()
                                            .resolute(std::declval<T &&>()))>
  constexpr variant &
  operator=(T &&t) noexcept(std::is_nothrow_constructible_v<U, T &&>
                                &&std ::is_nothrow_assignable_v<U, T &&>)
  {
    if (detail::find_type_index<U, Types...>() == index())
    {
      *static_cast<U *>(unsafe_get<U>(this)) = std::forward<T>(t);
      return *this;
    }

    // @fixme: can use copy and swap idiom to ensure strong exception safety?

    this->reset();
    if constexpr (std::is_trivial_v<U>)
      *static_cast<T *>(unsafe_get<U>(this)) = t;
    else
      new (unsafe_get<U>(this)) U(std::forward<T>(t));
    set_index(detail::find_type_index<U, Types...>());
    return *this;
  }

#endif
  /// @}

  /// @name Observers
  /// @{
  constexpr std::size_t index() const noexcept
  {
    if (base_type::index_ == -1)
      return variant_npos;
    return base_type::index_;
  }

  constexpr bool valueless_by_exception() const noexcept
  {
    return index() == variant_npos;
  }
  /// @}

  /// @name Modifiers
  /// @{
  template <typename T, typename... Args>
  constexpr T &emplace(Args &&... args)
  {
    return emplace<detail::find_type_index<T, Types...>::value>(
        std::forward<Args>(args)...);
  }

  template <typename T, typename U, typename... Args>
  constexpr T &emplace(std::initializer_list<U> il, Args &&... args)
  {
    return emplace<detail::find_type_index<T, Types...>::value>(
        il, std::forward<Args>(args)...);
  }

  template <typename std::size_t I, typename... Args>
  constexpr variant_alternative_t<I, variant> &emplace(Args &&... args)
  {
    using type = variant_alternative_t<I, variant>;
    base_type::reset();
    void *address = detail::unsafe_get<type>(this);
    auto pointer = new (address) type(std::forward<Args>(args)...);
    set_index(I);
    return *pointer;
  }

  template <typename std::size_t I, typename U, typename... Args>
  constexpr variant_alternative_t<I, variant> &
  emplace(std::initializer_list<U> il, Args &&... args)
  {
    using type = variant_alternative_t<I, variant>;
    base_type::reset();
    void *address = detail::unsafe_get<type>(this);
    auto pointer = new (address) type(il, std::forward<Args>(args)...);
    set_index(I);
    return *pointer;
  }

  constexpr void
  swap(variant &other) noexcept(((std::is_nothrow_move_constructible_v<Types> &&
                                  std::is_nothrow_swappable_v<Types>)&&...));
  /// @}
};

/// @name Non-member functions
/// @relates gpcl::variant
/// @{

template <typename T, typename... Types>
constexpr bool holds_alternative(const variant<Types...> &v) noexcept
{
  return detail::find_type_index<T, Types...>::value == v.index();
}

template <std::size_t I, class... Types>
constexpr variant_alternative_t<I, variant<Types...>> &get(variant<Types...> &v)
{
  if (v.index() != I)
    GPCL_THROW(bad_variant_access());

  using type = variant_alternative_t<I, variant<Types...>>;
  return *std::launder(unsafe_get<type>(&v));
}

template <std::size_t I, class... Types>
constexpr variant_alternative_t<I, variant<Types...>> &&
get(variant<Types...> &&v)
{
  if (v.index() != I)
    GPCL_THROW(bad_variant_access());

  using type = variant_alternative_t<I, variant<Types...>>;
  return std::move(*std::launder(unsafe_get<type>(&v)));
}

template <std::size_t I, class... Types>
constexpr const variant_alternative_t<I, variant<Types...>> &
get(const variant<Types...> &v)
{
  if (v.index() != I)
    GPCL_THROW(bad_variant_access());

  using type = std::add_const_t<variant_alternative_t<I, variant<Types...>>>;
  return *std::launder(unsafe_get<type>(&v));
}

template <std::size_t I, class... Types>
constexpr const variant_alternative_t<I, variant<Types...>> &&
get(const variant<Types...> &&v)
{
  if (v.index() != I)
    GPCL_THROW(bad_variant_access());

  using type = std::add_const_t<variant_alternative_t<I, variant<Types...>>>;
  return std::move(*std::launder(unsafe_get<type>(&v)));
}

template <typename T, typename... Types>
constexpr T &get(variant<Types...> &v)
{
  if (!holds_alternative<T>(v))
    GPCL_THROW(bad_variant_access());
  return *unsafe_get<T>(&v);
}

template <typename T, typename... Types>
constexpr T &&get(variant<Types...> &&v)
{
  if (!holds_alternative<T>(v))
    GPCL_THROW(bad_variant_access());
  return std::move(*unsafe_get<T>(&v));
}

template <typename T, typename... Types>
constexpr const T &get(const variant<Types...> &v)
{
  if (!holds_alternative<T>(v))
    GPCL_THROW(bad_variant_access());
  return *unsafe_get<T>(&v);
}

template <typename T, typename... Types>
constexpr const T &&get(const variant<Types...> &&v)
{
  if (!holds_alternative<T>(v))
    GPCL_THROW(bad_variant_access());
  return std::move(*unsafe_get<T>(&v));
}

template <std::size_t I, typename... Types>
constexpr variant_alternative_t<I, variant<Types...>> *
get_if(variant<Types...> *pv) noexcept
{
  using type = variant_alternative_t<I, variant<Types...>>;
  if (!holds_alternative<type>(*pv))
    return nullptr;
  return unsafe_get<type>(pv);
}

template <std::size_t I, typename... Types>
constexpr variant_alternative_t<I, variant<Types...>> const *
get_if(variant<Types...> const *pv) noexcept
{
  using type = variant_alternative_t<I, variant<Types...>>;
  if (!holds_alternative<type>(*pv))
    return nullptr;
  return unsafe_get<type>(pv);
}

template <typename T, typename... Types>
constexpr T *get_if(variant<Types...> *pv) noexcept
{
  if (!holds_alternative<T>(*pv))
    return nullptr;
  return unsafe_get<T>(pv);
}

template <typename T, typename... Types>
constexpr T const *get_if(variant<Types...> const *pv) noexcept
{
  if (!holds_alternative<T>(*pv))
    return nullptr;
  return unsafe_get<T>(pv);
}

namespace detail {

struct visit_impl
{
  template <typename Visitor>
  constexpr decltype(auto) operator()(Visitor &&visitor) const
  {
    return std::invoke(std::forward<Visitor>(visitor));
  }

  template <typename Visitor, typename Variant1, typename... Variants>
  constexpr decltype(auto) operator()(Visitor &&visitor, Variant1 &&variant1,
                                      Variants &&... variants) const
  {
    return (*this)(
        [&](auto &&... values) {
          return apply_visitor(
              [&](auto &&value1) {
                return std::invoke(std::forward<Visitor>(visitor),
                                   std::forward<decltype(value1)>(value1),
                                   std::forward<decltype(values)>(values)...);
              },
              std::forward<Variant1>(variant1));
        },
        std::forward<Variants>(variants)...);
  }

private:
  template <typename R, typename Visitor, typename Variant, bool False = false>
  static constexpr R apply_visitor_impl(Visitor &&, Variant &&,
                                        std::index_sequence<>)
  {
    GPCL_THROW(bad_variant_access());
  }

  template <typename R, typename Visitor, typename Variant, std::size_t I,
            std::size_t... Is>
  static constexpr R apply_visitor_impl(Visitor &&visitor, Variant &&variant_,
                                        std::index_sequence<I, Is...>)
  {
    if (variant_.index() == I)
      return std::invoke(std::forward<Visitor>(visitor),
                         get<I>(std::forward<Variant>(variant_)));
    return apply_visitor_impl<R>(std::forward<Visitor>(visitor),
                                 std::forward<Variant>(variant_),
                                 std::index_sequence<Is...>{});
  }

  template <typename Visitor, typename Variant>
  static constexpr decltype(auto) apply_visitor(Visitor &&visitor,
                                                Variant &&variant_)
  {
    using type_0 = variant_alternative_t<0, Variant>;
    using result_type = std::invoke_result_t<Visitor, type_0>;
    constexpr auto index_sequence =
        std::make_index_sequence<variant_size_v<Variant>>();

    return apply_visitor_impl<result_type>(std::forward<Visitor>(visitor),
                                           std::forward<Variant>(variant_),
                                           index_sequence);
  }
};
} // namespace detail

#if defined GPCL_DOXYGEN
template <typename Visitor, typename... Variants>
constexpr decltype(auto) visit(Visitor &&visitor, Variants &&... variants);
#else
constexpr detail::visit_impl visit{};
#endif

#if defined GPCL_DOXYGEN
template <typename T, typename... Types>
T *unsafe_get(variant<Types...> *v) noexcept;

template <typename T, typename... Types>
const T *unsafe_get(const variant<Types...> *v) noexcept;
#endif

/// @}

/// @name Relational operators
/// @{

namespace detail {

enum class compare_result
{
  equal = 0,
  less = -1,
  greater = 1,
};

template <typename T>
constexpr compare_result compare(const T &x, const T &y)
{
  if (x == y)
    return compare_result::equal;

  if (x < y)
    return compare_result::less;

  return compare_result::greater;
}

template <typename... Types>
constexpr compare_result compare(const variant<Types...> &x,
                                 const variant<Types...> &y)
{
  if (x.valueless_by_exception() && y.valueless_by_exception())
    return compare_result::equal;

  if (x.valueless_by_exception())
    return compare_result::less;

  if (y.valueless_by_exception())
    return compare_result::greater;

  if (x.index() != y.index())
    return compare(x.index(), y.index());

  return visit(
      [](auto const &a, auto const &b) {
        if constexpr (std::is_same_v<decltype(a), decltype(b)>)
          return compare(a, b);
        GPCL_UNREACHABLE("x and y should holds the same type");
      },
      x, y);
}
} // namespace detail

template <typename... Types>
constexpr bool operator==(const variant<Types...> &x,
                          const variant<Types...> &y)
{
  return compare(x, y) == detail::compare_result::equal;
}

template <typename... Types>
constexpr bool operator!=(const variant<Types...> &x,
                          const variant<Types...> &y)
{
  return compare(x, y) != detail::compare_result::equal;
}

template <typename... Types>
constexpr bool operator<(const variant<Types...> &x, const variant<Types...> &y)
{
  return compare(x, y) == detail::compare_result::less;
}

template <typename... Types>
constexpr bool operator>(const variant<Types...> &x, const variant<Types...> &y)
{
  return compare(x, y) == detail::compare_result::greater;
}

template <typename... Types>
constexpr bool operator<=(const variant<Types...> &x,
                          const variant<Types...> &y)
{
  return compare(x, y) != detail::compare_result::greater;
}

template <typename... Types>
constexpr bool operator>=(const variant<Types...> &x,
                          const variant<Types...> &y)
{
  return compare(x, y) != detail::compare_result::less;
}

/// @}

struct monostate
{
};

namespace detail {
inline constexpr compare_result compare(const monostate &,
                                        const monostate &) noexcept
{
  return compare_result::equal;
}
} // namespace detail

#ifndef GPCL_DOXYGEN
template <typename... Types>
constexpr void variant<Types...>::swap(variant &other) noexcept(
    ((std::is_nothrow_move_constructible_v<Types> &&
      std::is_nothrow_swappable_v<Types>)&&...))
{
  if (valueless_by_exception() && other.valueless_by_exception())
  {
    // do nothing
  }
  else if (index() == other.index())
  {
    visit(
        [](auto &x, auto &y) {
          if constexpr (std::is_same_v<decltype(x), decltype(y)>)
          {
            gpcl::swap(x, y);
            return;
          }
          GPCL_UNREACHABLE("x and y should be the same type");
        },
        *this, other);
  }
  else
  {
    visit(
        [&](auto &x, auto &y) {
          auto tmp = std::move(y);
          other = std::move(x);
          *this = std::move(tmp);
        },
        *this, other);
  }
}
#endif

namespace swap_detail {
template <typename... Types>
void swap(variant<Types...> &x,
          variant<Types...> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}
} // namespace swap_detail

} // namespace gpcl

#endif // GPCL_VARIANT_HPP
