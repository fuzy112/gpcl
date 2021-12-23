//
// shared_ptr.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SHARED_PTR_HPP
#define GPCL_SHARED_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/ref_count.hpp>
#include <gpcl/swap.hpp>

namespace gpcl {

template <typename T>
class weak_ptr;

template <typename T>
class shared_ptr;

namespace detail {

struct nullptr_wrapper
{
  constexpr nullptr_wrapper() noexcept = default;

  constexpr nullptr_wrapper(std::nullptr_t) noexcept {}

  constexpr operator std::nullptr_t() const noexcept { return nullptr; }

  constexpr explicit operator bool() const noexcept { return false; }
};

} // namespace detail

template <typename Deleter, typename T>
Deleter *get_deleter(const shared_ptr<T> &p) noexcept;

namespace detail {
class enable_shared_from_this_base;
}

/** @addtogroup smart_pointer Smart Pointers
 *  @{
 */

/// A Reference-counting based smart pointer.
template <typename T>
class shared_ptr
{
  template <typename Y>
  friend class weak_ptr;

  template <typename Y>
  friend class shared_ptr;

  T *p_ = nullptr;
  detail::ref_count_base *s_ = nullptr;

  template <typename Deleter, typename Y>
  friend Deleter *get_deleter(const shared_ptr<Y> &p) noexcept;

  template <typename Y>
  void enables_shared_from_this(Y *ptr) noexcept;

public:
  using element_type = std::remove_extent_t<T>;
  using weak_type = std::weak_ptr<T>;

  /// @name Constructors and Destructor
  /// @{

  /// Default constructor.
  constexpr shared_ptr() noexcept = default;

  /// Constructs empty shared_ptr.
  constexpr shared_ptr(std::nullptr_t) noexcept {}

  /// Constructs a new shared_ptr that manages the object pointed to by @c ptr.
  /**
   * @note When no more shared_ptr links to it, the managed object will be
   * destructed using delete expression.
   *
   * @throws std::bad_alloc if required additional memory could not be obtained.
   *
   * If an exception occurrs, this calls `delete ptr`.
   */
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  explicit shared_ptr(Y *ptr)
      : p_(ptr),
        s_(detail::ref_count_ptr<Y *, default_delete<Y>, std::allocator<Y *>>::
               create(std::allocator<Y *>(), ptr, default_delete<Y>()))
  {
    enables_shared_from_this(ptr);
  }

  /// Constructs a new shared_ptr that manages the object pointed to by @c ptr.
  /**
   * @note When no more shared_ptr links to it, the managed object will be
   * destructed using @c d.
   *
   * @throws std::bad_alloc if required additional memory could not be
   * obtained.
   *
   * If an exception occurrs, this calls `d(ptr)`.
   */
  template <typename Y, typename Deleter,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(Y *ptr, Deleter d)
      : p_(ptr),
        s_(detail::ref_count_ptr<Y *, Deleter, std::allocator<Y *>>::create(
            std::allocator<Y *>(), ptr, std::move(d)))
  {
    enables_shared_from_this(ptr);
  }

  /// Constructs a new shared_ptr that manages no object.
  /**
   * @note When no more shared_ptr links to it, the managed object will be
   * destructed using @c d, if any.
   *
   * @throws std::bad_alloc if required additional memory could not be
   * obtained.
   *
   * If an exception occurrs, this calls `d(ptr)`.
   */
  template <typename Deleter>
  shared_ptr(std::nullptr_t, Deleter d)
      : s_(detail::ref_count_ptr<detail::nullptr_wrapper, Deleter,
                                 std::allocator<std::nullptr_t>>::
               create(std::allocator<std::nullptr_t>(),
                      detail::nullptr_wrapper(), std::move(d)))
  {
  }

  /// Constructs a new shared_ptr that manages the object pointed to by @c ptr.
  /**
   * @note When no more shared_ptr links to it, the managed object will be
   * destructed using @c d.
   *
   * @note Memory used internally is allocated using @c alloc.
   *
   * @throws std::bad_alloc if required additional memory could not be
   * obtained.
   *
   * If an exception occurrs, this calls `d(ptr)`.
   */
  template <typename Y, typename Deleter, typename Alloc,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(Y *ptr, Deleter d, Alloc alloc)
      : p_(ptr),
        s_(detail::ref_count_ptr<Y *, Deleter, Alloc>::create(alloc, ptr,
                                                              std::move(d)))
  {
    enables_shared_from_this(ptr);
  }

  /// Constructs a new shared_ptr that manages the no object.
  /**
   * @note When no more shared_ptr links to it, the managed object will be
   * destructed using @c d.
   *
   * @note Memory used internally is allocated using @c alloc.
   *
   * @throws std::bad_alloc if required additional memory could not be
   * obtained.
   *
   * If an exception occurrs, this calls `d(ptr)`.
   */
  template <typename Deleter, typename Alloc>
  shared_ptr(std::nullptr_t, Deleter d, Alloc alloc)
      : s_(detail::ref_count_ptr<detail::nullptr_wrapper, Deleter,
                                 Alloc>::create(alloc,
                                                detail::nullptr_wrapper(),
                                                std::move(d)))
  {
  }

  template <typename Y>
  shared_ptr(const shared_ptr<Y> &r, element_type *ptr,
             bool enables_shared_from_this = false) noexcept
      : p_(ptr),
        s_(r.s_)
  {
    if (s_)
      s_->get();
    if (enables_shared_from_this)
      this->enables_shared_from_this(ptr);
  }

  template <typename Y>
  shared_ptr(shared_ptr<Y> &&r, element_type *ptr,
             bool enables_shared_from_this = false) noexcept
      : p_(ptr),
        s_(detail::exchange(r.s_, nullptr))
  {
    r.p_ = nullptr;
    if (enables_shared_from_this)
      this->enables_shared_from_this(ptr);
  }

  /// Copy constructor.
  shared_ptr(const shared_ptr &r) noexcept : p_(r.p_), s_(r.s_)
  {
    if (s_)
      s_->get();
  }

  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(const shared_ptr<Y> &r) noexcept : p_(r.p_),
                                                s_(r.s_)
  {
    if (s_)
      s_->get();
  }

  /// Move constructor.
  shared_ptr(shared_ptr &&r) noexcept
      : p_(detail::exchange(r.p_, nullptr)),
        s_(detail::exchange(r.s_, nullptr))
  {
  }

  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(shared_ptr<Y> &&r) noexcept
      : p_(detail::exchange(r.p_, nullptr)),
        s_(detail::exchange(r.s_, nullptr))
  {
  }

  /// Get a strong reference to the object pointed to by @c r.
  /**
   * @throws bad_weak_ptr if @c r.expired() is @c true.
   */
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  explicit shared_ptr(const weak_ptr<Y> &r);

  /// Create a shared_ptr that manages the object currently managed by @c r.
  /**
   * @post @c bool(r) is @c false.
   */
  template <typename Y, typename Deleter,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(unique_ptr<Y, Deleter> &&r)
      : shared_ptr(r.release(), std::move(r.get_deleter()))
  {
  }



  /// Destructs the owned object if no more `shared_ptr`s link to it.
  ~shared_ptr()
  {
    if (s_)
      s_->put();
  }

  /// @}

  /// @name Assignment Operators
  /// @{

  /// Assigns the shared_ptr.
  shared_ptr &operator=(const shared_ptr &r) noexcept
  {
    shared_ptr(r).swap(*this);
    return *this;
  }

  /// Assigns the shared_ptr.
  template <typename Y>
  shared_ptr &operator=(const shared_ptr<Y> &r) noexcept
  {
    shared_ptr(r).swap(*this);
    return *this;
  }

  /// Assigns the shared_ptr.
  shared_ptr &operator=(shared_ptr &&r) noexcept
  {
    r.swap(*this);
    return *this;
  }

  /// Assigns the shared_ptr.
  template <typename Y>
  shared_ptr &operator=(shared_ptr<Y> &&r) noexcept
  {
    shared_ptr(std::move(r)).swap(*this);
    return *this;
  }

  /// Assigns the shared_ptr.
  template <typename Y, typename Deleter>
  shared_ptr &operator=(unique_ptr<Y, Deleter> &&r)
  {
    shared_ptr(std::move(r)).swap(*this);
    return *this;
  }

  /// @}

  /// @name Modifiers
  /// @{

  /// Releases the ownership of the managed object, if any.
  void reset() noexcept { shared_ptr().swap(*this); }

  /// Replaces the managed object with the object pointed to by @c ptr.
  /**
   * @throws std::bad_alloc if required additional memory could not be
   * obtained.
   *
   * If an exception occurrs, `delete ptr` is called and the state of `this` is
   * not changed.
   */
  template <typename Y>
  void reset(Y *ptr)
  {
    shared_ptr(ptr).swap(*this);
  }

  /// Replaces the managed object with the object pointed to by @c ptr.
  /**
   * @throws std::bad_alloc if required additional memory could not be
   * obtained.
   *
   * If an exception occurrs, `d(ptr)` is called and the state of `this` is not
   * changed.
   */
  template <typename Y, typename Deleter>
  void reset(Y *ptr, Deleter d)
  {
    shared_ptr(ptr, d).swap(*this);
  }

  /// Replaces the managed object with the object pointed to by @c ptr.
  /** @throws std::bad_alloc if required additional memory could not be
   * obtained.
   *
   * If an exception occurrs, `d(ptr)` is called and the state of `this` is not
   * changed.
   */
  template <typename Y, typename Deleter, typename Alloc>
  void reset(Y *ptr, Deleter d, Alloc alloc)
  {
    shared_ptr(ptr, std::move(d), alloc).swap(*this);
  }

  /// Swaps two `shared_ptr`s.
  void swap(shared_ptr &r) noexcept
  {
    using gpcl::swap;
    swap(p_, r.p_);
    swap(s_, r.s_);
  }

  /// @}

  /// @name Observers
  /// @{

  /// Returns the stored pointer.
  element_type *get() const noexcept { return p_; }

  /// Derferences the stored pointer.
  /**
   * @return The result of dereferencing the stored pointer.
   *
   * @pre The stored pointer must not be null.
   */
  template <typename U = T>
  std::enable_if_t<!std::is_void_v<U>, element_type> &operator*() const noexcept
  {
    GPCL_ASSERT(get());
    return *get();
  }

  /// Returns the stored pointer.
  /**
   * @return The stored pointer.
   *
   * @pre The stored pointer must not be null.
   */
  element_type *operator->() const noexcept
  {
    GPCL_ASSERT(get());
    return get();
  }

  /// @returns the number of shared_ptr objects referring to the same managed
  /// object.
  long use_count() const noexcept
  {
    if (s_)
      return s_->use_count();
    return 0;
  }

  explicit operator bool() const noexcept { return get() != nullptr; }

  /// @}

  template <typename Y>
  bool owner_before(const shared_ptr<Y> &other) const noexcept
  {
    return s_ - other.s_ < 0;
  }

  template <typename Y>
  bool owner_before(const weak_ptr<Y> &other) const noexcept;
};

/// @name Comparators
/// @relates gpcl::shared_ptr
/// @{

template <class T1, class T2>
bool operator==(const shared_ptr<T1> &x, const shared_ptr<T2> &y)
{
  return x.get() == y.get();
}

template <class T1, class T2>
bool operator!=(const shared_ptr<T1> &x, const shared_ptr<T2> &y)
{
  return x.get() != y.get();
}

template <class T1, class T2>
bool operator<(const shared_ptr<T1> &x, const shared_ptr<T2> &y)
{
  using CT = typename std::common_type<T1 *, T2 *>::type;

  return std::less<CT>()(x.get(), y.get());
}

template <class T1, class T2>
bool operator<=(const shared_ptr<T1> &x, const shared_ptr<T2> &y)
{
  return !(y < x);
}

template <class T1, class T2>
bool operator>(const shared_ptr<T1> &x, const shared_ptr<T2> &y)
{
  return y < x;
}

template <class T1, class T2>
bool operator>=(const shared_ptr<T1> &x, const shared_ptr<T2> &y)
{
  return !(x < y);
}

template <class T>
bool operator==(const shared_ptr<T> &x, std::nullptr_t) noexcept
{
  return !x;
}

template <class T>
bool operator==(std::nullptr_t, const shared_ptr<T> &x) noexcept
{
  return !x;
}

template <class T>
bool operator!=(const shared_ptr<T> &x, std::nullptr_t) noexcept
{
  return x;
}

template <class T>
bool operator!=(std::nullptr_t, const shared_ptr<T> &x) noexcept
{
  return x;
}

template <class T>
bool operator<(const shared_ptr<T> &x, std::nullptr_t)
{
  return std::less<typename shared_ptr<T>::pointer>()(x.get(), nullptr);
}

template <class T>
bool operator<(std::nullptr_t, const shared_ptr<T> &y)
{
  return std::less<typename shared_ptr<T>::pointer>()(nullptr, y.get());
}

template <class T>
bool operator<=(const shared_ptr<T> &x, std::nullptr_t)
{
  return !(nullptr < x);
}

template <class T>
bool operator<=(std::nullptr_t, const shared_ptr<T> &y)
{
  return !(y < nullptr);
}

template <class T>
bool operator>(const shared_ptr<T> &x, std::nullptr_t)
{
  return nullptr < x;
}

template <class T>
bool operator>(std::nullptr_t, const shared_ptr<T> &y)
{
  return y < nullptr;
}

template <class T>
bool operator>=(const shared_ptr<T> &x, std::nullptr_t)
{
  return !(x < nullptr);
}

template <class T>
bool operator>=(std::nullptr_t, const shared_ptr<T> &y)
{
  return !(nullptr < y);
}

/// @}

namespace swap_detail {
template <typename T>
void swap(shared_ptr<T> &x, shared_ptr<T> &y) noexcept
{
  x.swap(y);
}
} // namespace swap_detail

/// @name Generic Pointer Casts
/// @relates gpcl::shared_ptr
/// @{

template <typename T, typename Y>
shared_ptr<T> static_pointer_cast(const shared_ptr<Y> &p) noexcept
{
  return shared_ptr<T>(p, static_cast<T *>(p.get()));
}

template <typename T, typename U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U> &p) noexcept
{
  return shared_ptr<T>(p, dynamic_cast<T *>(p.get()));
}

template <typename T, typename U>
shared_ptr<T> const_pointer_cast(const shared_ptr<U> &p) noexcept
{
  return shared_ptr<T>(p, const_cast<T *>(p.get()));
}

template <typename T, typename U>
shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<U> &p) noexcept
{
  return shared_ptr<T>(p, reinterpret_cast<T *>(p.get()));
}

/// @}

/// @}

} // namespace gpcl

#include <gpcl/allocate_shared.hpp>
#include <gpcl/get_deleter.hpp>
#include <gpcl/make_shared.hpp>

#include <gpcl/impl/shared_ptr.hpp>

#endif // GPCL_SHARED_PTR_HPP
