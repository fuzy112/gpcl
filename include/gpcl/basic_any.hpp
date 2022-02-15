//
// basic_any.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2021-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BASIC_ANY_HPP
#define GPCL_BASIC_ANY_HPP

#include <gpcl/detail/any_manager.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/in_place_type.hpp>
#include <gpcl/typeid.hpp>

namespace gpcl {

template <std::size_t LocalSize = 3 * sizeof(void *),
          std::size_t LocalAlign = alignof(std::max_align_t)>
class basic_any;

template <typename T>
struct is_basic_any : std::false_type
{
};

template <std::size_t LocalSize, std::size_t LocalAlign>
struct is_basic_any<basic_any<LocalSize, LocalAlign>> : std::true_type
{
};

template <typename T>
constexpr is_basic_any<T> is_basic_any_v{};

/// @see gpcl::any
template <std::size_t LocalSize, std::size_t LocalAlign>
class basic_any
{
  using any_data_t = detail::any_data<LocalSize, LocalAlign>;
  using any_manage_func_t = detail::any_manage_func_t<LocalSize, LocalAlign>;
  using any_manage_op = detail::any_manage_op;

  template <typename T>
  using any_manager = detail::any_manager<T, LocalSize, LocalAlign>;

  any_data_t data_{};
  any_manage_func_t manage_ = nullptr;

public:
  /// @name Constructors
  /// @{

  constexpr basic_any() = default;

  /// Copy constructor.
  basic_any(const basic_any &other) : manage_(other.manage_)
  {
    if (manage_)
      manage_(&data_, &other.data_, any_manage_op::clone);
  }

  /// Move constructor.
  basic_any(basic_any &&other) noexcept : manage_(other.manage_)
  {
    if (manage_)
      if (manage_(&data_, &other.data_, any_manage_op::move))
        other.manage_ = nullptr;
  }

  /// Construct a new basic_any.
  template <typename ValueType,
            std::enable_if_t<!is_basic_any_v<std::decay_t<ValueType>>, int> = 0>
  basic_any(ValueType &&value)
      : manage_(&any_manager<std::decay_t<ValueType>>::manage)
  {
    using decayed_value_t = std::decay_t<ValueType>;
    using manager = any_manager<decayed_value_t>;

    if constexpr (manager::local_storage)
      new (&data_.local_buffer) decayed_value_t(std::forward<ValueType>(value));
    else
      data_.remote_addr = new decayed_value_t(std::forward<ValueType>(value));
  }

  /// Construct a new basic_any.
  template <typename ValueType, typename... Args>
  explicit basic_any(in_place_type_t<ValueType>, Args &&...args)
      : manage_(&any_manager<std::decay_t<ValueType>>::manage)
  {
    using decayed_value_t = std::decay_t<ValueType>;
    using manager = any_manager<decayed_value_t>;

    if constexpr (manager::local_storage)
      new (&data_.local_buffer) decayed_value_t(std::forward<Args>(args)...);
    else
      data_.remote_addr = new decayed_value_t(std::forward<Args>(args)...);
  }

  /// Construct a new basic_any.
  template <typename ValueType, typename U, typename... Args>
  explicit basic_any(in_place_type_t<ValueType>, std::initializer_list<U> il,
                     Args &&...args)
      : manage_(&any_manager<std::decay_t<ValueType>>::manage)
  {
    using decayed_value_t = std::decay_t<ValueType>;
    using manager = any_manager<decayed_value_t>;

    if constexpr (manager::local_storage)
      new (&data_.local_buffer)
          decayed_value_t(il, std::forward<Args>(args)...);
    else
      data_.remote_addr = new decayed_value_t(il, std::forward<Args>(args)...);
  }

  /// @}

  /// @name Destructor
  ~basic_any()
  {
    if (manage_)
      manage_(&data_, nullptr, any_manage_op::destroy);
  }

  /// @name Assignment Operators
  /// @{

  /// Copy assignment.
  basic_any &operator=(const basic_any &other)
  {
    basic_any(other).swap(*this);
    return *this;
  }

  /// Move assignment.
  basic_any &operator=(basic_any &&other) noexcept
  {
    if (manage_)
      manage_(&data_, nullptr, any_manage_op::destroy);
    manage_ = other.manage_;
    if (manage_)
      if (manage_(&data_, &other.data_, any_manage_op::move))
        other.manage_ = nullptr;
    return *this;
  }

  /// Replace the contained value.
  template <typename ValueType,
            std::enable_if_t<
                std::is_copy_constructible<std::decay_t<ValueType>>::value &&
                    !is_basic_any_v<std::decay_t<ValueType>>,
                int> = 0>
  basic_any &operator=(ValueType &&value)
  {
    basic_any(std::forward<ValueType>(value)).swap(*this);
    return *this;
  }

  /// @}

  /// @name Modifiers
  /// @{

  /// Swap two `basic_any`s.
  void swap(basic_any &other) noexcept
  {
    basic_any temp(std::move(other));
    other = std::move(*this);
    *this = std::move(temp);
  }

  /// Destroyes the contained value.
  void reset() noexcept
  {
    if (manage_)
      manage_(&data_, nullptr, any_manage_op::destroy);
    manage_ = nullptr;
  }

  /// Constructs a contained value.
  template <typename ValueType, typename... Args>
  std::decay_t<ValueType> &emplace(Args &&...args)
  {
    basic_any(in_place_type<ValueType>, std::forward<Args>(args)...)
        .swap(*this);
    return *static_cast<std::decay_t<ValueType> *>(raw_value());
  }

  /// Constructs a contained value.
  template <typename ValueType, typename U, typename... Args>
  std::decay_t<ValueType> &emplace(std::initializer_list<U> il, Args &&...args)
  {
    basic_any(in_place_type<ValueType>, il, std::forward<Args>(args)...)
        .swap(*this);
    return *static_cast<std::decay_t<ValueType> *>(raw_value());
  }

  /// @}

  /// @name Observers
  /// @{

#ifndef GPCL_DOXYGEN
  const void *raw_value() const noexcept
  {
    return manage_(nullptr, &data_, any_manage_op::get_pointer);
  }

  void *raw_value() noexcept
  {
    return manage_(nullptr, &data_, any_manage_op::get_pointer);
  }
#endif

  /// Determines if the basic_any is empty.
  bool empty() const noexcept { return !manage_; }

  /// Determines if the basic_any contains a value.
  bool has_value() const noexcept { return manage_; }

  /// Returns the type info of the contained value.
  const type_info &type() const noexcept
  {
    if (manage_)
      return *static_cast<const type_info *>(
          manage_(nullptr, &data_, any_manage_op::get_type_info));
    return typeid_<void>();
  }

  /// @}
};

/// Determines if the basic_any contains a value of type `T`.
/// @relates gpcl::basic_any
template <typename T, std::size_t S, std::size_t A>
bool holds_type(const basic_any<S, A> &a)
{
  return a.type() == typeid_<T>();
}

template <std::size_t LocalSize, std::size_t LocalAlign>
inline void swap(basic_any<LocalSize> &x, basic_any<LocalAlign> &y) noexcept
{
  x.swap(y);
}

} // namespace gpcl

#include <gpcl/any_cast.hpp>
#include <gpcl/make_basic_any.hpp>

#endif // GPCL_BASIC_ANY_HPP
