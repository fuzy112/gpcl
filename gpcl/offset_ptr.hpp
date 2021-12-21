//
// offset_ptr.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OFFSET_PTR_HPP
#define GPCL_OFFSET_PTR_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/narrow_cast.hpp>

#include <limits>
#include <string>

namespace gpcl {

template <typename T>
class offset_ptr;

template <typename T>
struct is_offset_ptr : std::false_type
{
};

template <typename T>
struct is_offset_ptr<offset_ptr<T>> : std::true_type
{
};

namespace detail {
template <typename T, typename = void>
class offset_ptr_base
{
public:
  typedef T &reference;
};

template <typename T>
class offset_ptr_base<T, typename std::enable_if<std::is_void<T>::value>::type>
{
public:
};

template <typename T>
std::intptr_t as_integer(T *ptr)
{
  return reinterpret_cast<std::intptr_t>(ptr);
}

} // namespace detail

/// A fancy pointer class that stores offset instead of virtual address in it.
template <typename T>
class offset_ptr : public detail::offset_ptr_base<T>
{
private:
  using iterator_category = std::random_access_iterator_tag;

  using value_type = typename std::decay<T>::type;
  typedef T *pointer;
  std::size_t typedef size_type;
  std::ptrdiff_t typedef difference_type;
  std::intptr_t typedef offset_type;

  static constexpr const offset_type invalid_offset =
      (std::numeric_limits<offset_type>::max)();
  offset_type offset_; // offset in bytes to this

public:
  /// Constructor
  inline constexpr offset_ptr(std::nullptr_t = nullptr) noexcept
      : offset_(invalid_offset)
  {
  }

  /// Constructor
  offset_ptr(T *ptr);

  /// Copy constructor
  inline constexpr offset_ptr(const offset_ptr &other) noexcept
      : offset_ptr(other.get())
  {
  }

  /// Converts to raw pointer
  inline explicit operator pointer() const noexcept { return get(); }

  /// Copy assignment
  inline offset_ptr &operator=(const offset_ptr &other);

  /// Converts pointer difference to offset
  inline static constexpr offset_type diff_to_offset(difference_type diff);

  /// Converts offset to pointer difference
  inline static constexpr difference_type offset_to_diff(offset_type offset);

  /// Dereference
  /// @return reference to the value
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value, int>::type = 0>
  inline auto &operator*() const;

  /// Member access operator
  /// @return a raw pointer
  inline pointer operator->() const;

  inline pointer get() const;

  /// Increment the offset_ptr
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline offset_ptr &operator++() &;

  /// Increment the offset_ptr and returns the original value
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  [[nodiscard]] inline const offset_ptr operator++(int) &;

  /// Decrement the offset_ptr
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline offset_ptr &operator--() &;

  /// Decrement the offset_ptr and return the original value
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  [[nodiscard]] inline offset_ptr operator--(int) &;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline offset_ptr &operator+=(difference_type diff);

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline offset_ptr &operator-=(difference_type diff);

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline difference_type operator-(const offset_ptr &other) const;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline offset_ptr operator+(difference_type diff) const;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline offset_ptr operator-(difference_type diff) const;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  friend inline offset_ptr operator+(difference_type diff,
                                     const offset_ptr &ptr) noexcept
  {
    return ptr + diff;
  }

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  inline U &operator[](difference_type diff) const noexcept
  {
    return *(*this + diff);
  }

  inline bool operator==(const offset_ptr &other) const noexcept
  {
    return this->get() == other.get();
  }

  inline bool operator!=(const offset_ptr &other) const noexcept
  {
    return !(*this == other);
  }

  inline bool operator==(std::nullptr_t) const noexcept
  {
    return this->get() == nullptr;
  }

  inline bool operator!=(std::nullptr_t) const noexcept
  {
    return this->get() != nullptr;
  }

  inline bool operator==(pointer ptr) const noexcept
  {
    return this->get() == ptr;
  }

  inline bool operator!=(pointer ptr) const noexcept
  {
    return this->get() == ptr;
  }

  template <
      typename Dummy = T,
      typename std::enable_if<std::is_convertible<Dummy *, const char *>::value,
                              int>::type = 0>
  inline bool operator==(const std::string &str) const noexcept
  {
    GPCL_ASSERT(get() != 0);
    return get() == str;
  }

  friend inline bool operator<(const offset_ptr &x,
                               const offset_ptr &y) noexcept
  {
    return std::addressof(*x) < std::addressof(*y);
  }

  friend inline bool operator>(const offset_ptr &x,
                               const offset_ptr &y) noexcept
  {
    return y < x;
  }

  friend inline bool operator<=(const offset_ptr &x,
                                const offset_ptr &y) noexcept
  {
    return !(x > y);
  }

  friend inline bool operator>=(const offset_ptr &x,
                                const offset_ptr &y) noexcept
  {
    return !(x < y);
  }

  inline explicit operator bool() const noexcept { return get() != nullptr; }

  template <
      typename Dummy = T,
      typename std::enable_if<std::is_convertible<Dummy *, const char *>::value,
                              int>::type = 0>
  bool operator!=(const std::string &str) const;
};

template <typename T, typename U>
offset_ptr<T> static_pointer_cast(const offset_ptr<U> &p) noexcept
{
  return offset_ptr<T>(static_cast<U *>(p.get()));
}

template <typename T, typename U>
offset_ptr<T> dynamic_pointer_cast(const offset_ptr<U> &p) noexcept
{
  return offset_ptr<T>(dynamic_cast<U *>(p.get()));
}

template <typename T, typename U>
offset_ptr<T> const_pointer_cast(const offset_ptr<U> &p) noexcept
{
  return offset_ptr<T>(const_cast<U *>(p.get()));
}

template <typename T, typename U>
offset_ptr<T> reinterpret_pointer_cast(const offset_ptr<U> &p) noexcept
{
  return offset_ptr<T>(reinterpret_cast<U *>(p.get()));
}

} // namespace gpcl

#ifndef GPCL_STANDARDESE
#  include <gpcl/impl/offset_ptr.hpp>
#endif

#endif
