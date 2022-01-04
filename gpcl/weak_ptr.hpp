//
// weak_ptr.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_WEAK_PTR_HPP
#define GPCL_WEAK_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/ref_count_base.hpp>

namespace gpcl {

template <typename T>
class shared_ptr;

/// @addtogroup smart_pointer Smart Pointers
/// @{

template <typename T>
class weak_ptr
{
  template <typename Y>
  friend class weak_ptr;

  template <typename Y>
  friend class shared_ptr;

  T *p_ = nullptr;
  detail::ref_count_base *s_ = nullptr;

public:
  using element_type = T;

  /// Default constructor.
  constexpr weak_ptr() noexcept = default;

  /// Copy constructor.
  weak_ptr(const weak_ptr &r) noexcept : p_(r.p_), s_(r.s_)
  {
    if (s_)
      s_->weak_get();
  }

  /// Converting copy constructor.
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  weak_ptr(const weak_ptr<Y> &r) noexcept : p_(r.p_),
                                            s_(r.s_)
  {
    if (s_)
      s_->weak_get();
  }

  /// Construct a weak_ptr from a shared_ptr.
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  weak_ptr(const shared_ptr<Y> &r) noexcept;

  /// Move constructor.
  weak_ptr(weak_ptr &&r) noexcept
      : p_(detail::exchange(r.p_, nullptr)),
        s_(detail::exchange(r.s_, nullptr))
  {
  }

  /// Converting move constructor.
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  weak_ptr(weak_ptr<Y> &&r) noexcept
      : p_(detail::exchange(r.p_, nullptr)),
        s_(detail::exchange(r.s_, nullptr))
  {
  }

  /// Destructor.
  ~weak_ptr()
  {
    if (s_)
      s_->weak_put();

    p_ = nullptr;
    s_ = nullptr;
  }

  /// Copy assignment.
  weak_ptr &operator=(const weak_ptr &r) noexcept
  {
    weak_ptr(r).swap(*this);
    return *this;
  }

  /// Converting copy assignment.
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  weak_ptr &operator=(const weak_ptr<Y> &r) noexcept
  {
    weak_ptr(r).swap(*this);
    return *this;
  }

  /// Move assignment.
  weak_ptr &operator=(weak_ptr &&r) noexcept
  {
    r.swap(*this);
    return *this;
  }

  /// Converting move assignment.
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  weak_ptr &operator=(weak_ptr<Y> &&r) noexcept
  {
    weak_ptr(std::move(r)).swap(*this);
    return *this;
  }

  /// Assignment a shared_ptr to a weak_ptr.
  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  weak_ptr &operator=(const shared_ptr<Y> &r) noexcept;

  /// Reset to nullptr.
  void reset() noexcept
  {
    if (s_)
      s_->weak_put();

    p_ = nullptr;
    s_ = nullptr;
  }

  /// Swap two weak_ptr's.
  void swap(weak_ptr &r) noexcept
  {
    using gpcl::swap;
    swap(p_, r.p_);
    swap(s_, r.s_);
  }

  /// Returns number of shared_ptr to the managed object.
  long use_count() const noexcept
  {
    if (!s_)
      return 0;
    return s_->use_count();
  }

  /// Determines whether the managed object has been destroyed.
  bool expired() const noexcept
  {
    if (!s_)
      return true;

    return s_->use_count() == 0;
  }

  /// Try to create a shared_ptr to the managed object.
  shared_ptr<T> lock() const noexcept
  {
    shared_ptr<T> rv;
    if (!s_)
      return rv;

    if (!s_->lock())
      return rv;

    rv.p_ = p_;
    rv.s_ = s_;

    return rv;
  }

  /// Compare the addresses of the control blocks.
  template <typename Y>
  bool owner_before(const weak_ptr<Y> &other) const noexcept
  {
    return s_ < other.s_;
  }

  /// Compare the addresses of the control blocks.
  template <typename Y>
  bool owner_before(const shared_ptr<Y> &other) const noexcept;

#ifndef GPCL_DOXYGEN
  element_type *get_unchecked() const noexcept { return p_; }

  template <typename Y>
  weak_ptr<Y> generic_pointer_cast_helper(Y *p) const noexcept
  {
    if (!s_)
      return shared_ptr<T>();
    if (!p_)
      return shared_ptr<T>();
    weak_ptr<Y> rv;
    rv.p_ = p;
    rv.s_ = s_;
    rv.s_->weak_get();
    return rv;
  }
#endif
};

template <typename T, typename U>
weak_ptr<T> static_pointer_cast(const weak_ptr<U> &p) noexcept
{
  return p.generic_pointer_cast_helper(static_cast<T *>(p.get_unchecked()));
}

#if !defined GPCL_NO_RTTI
template <typename T, typename U>
weak_ptr<T> dynamic_pointer_cast(const weak_ptr<U> &p) noexcept;
#endif

template <typename T, typename U>
weak_ptr<T> const_pointer_cast(const weak_ptr<U> &p) noexcept
{
  return p.generic_pointer_cast_helper(const_cast<T *>(p.get_unchecked()));
}

template <typename T, typename U>
weak_ptr<T> reinterpret_pointer_cast(const weak_ptr<U> &p) noexcept
{
  return p.generic_pointer_cast_helper(
      reinterpret_cast<T *>(p.get_unchecked()));
}

/// @}

} // namespace gpcl

#include <gpcl/impl/weak_ptr.hpp>

#endif // GPCL_WEAK_PTR_HPP
