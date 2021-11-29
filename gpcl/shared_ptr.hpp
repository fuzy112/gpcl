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
#include <gpcl/detail/shared_block.hpp>
#include <gpcl/optional.hpp>
#include <gpcl/unique_ptr.hpp>

namespace gpcl {

namespace detail {
struct create_from_shared_block_t
{
};
constexpr create_from_shared_block_t create_from_shared_block;
} // namespace detail

template <typename T>
class weak_ptr;

template <typename T>
class shared_ptr;

namespace detail {

template <typename T, typename = void>
struct shared_ptr_hooks
{
  template <typename Y>
  static void on_creation(Y *, const shared_ptr<T> &) noexcept
  {
  }
};

} // namespace detail

/** @defgroup SmartPtr Smart Pointers.
*/

/// @ingroup SmartPtr
/// @{

/// Reference-counting based smart pointer.
template <typename T>
class shared_ptr
{
  template <typename Y>
  friend class weak_ptr;

  template <typename Y>
  friend class shared_ptr;

  T *p_ = nullptr;
  detail::shared_block_base *s_ = nullptr;

public:
  using element_type = T;

  // internal use.
  explicit shared_ptr(detail::create_from_shared_block_t, T *p,
                      detail::shared_block_base *c) noexcept
      : p_(p),
        s_(c)
  {
  }

  constexpr shared_ptr() noexcept = default;

  constexpr shared_ptr(std::nullptr_t) noexcept {}

  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  explicit shared_ptr(Y *ptr)
      : p_(ptr),
        s_(detail::shared_block<gpcl::unique_ptr<Y>>::create(ptr))
  {
    detail::shared_ptr_hooks<T>::on_creation(p_, *this);
  }

  template <typename Y, typename Deleter,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(Y *ptr, Deleter d)
      : p_(ptr),
        s_(detail::shared_block<gpcl::unique_ptr<Y, Deleter>>::create(
            ptr, std::move(d)))
  {
    detail::shared_ptr_hooks<T>::on_creation(p_, *this);
  }

  template <typename Deleter>
  shared_ptr(std::nullptr_t, Deleter d) : p_(nullptr),
                                          s_(nullptr)
  {
    (void)d;
  }

  template <typename Y, typename Deleter, typename Alloc,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(Y *ptr, Deleter d, Alloc alloc)
      : p_(ptr),
        s_(detail::shared_block<gpcl::unique_ptr<Y, Deleter>, Alloc>::create(
            alloc, ptr, std::move(d)))
  {
    detail::shared_ptr_hooks<T>::on_creation(p_, *this);
  }

  template <typename Deleter, typename Alloc>
  shared_ptr(std::nullptr_t, Deleter d, Alloc alloc) : p_(nullptr),
                                                       s_()
  {
    (void)d;
    (void)alloc;
  }

  template <typename Y>
  shared_ptr(const shared_ptr<Y> &r, element_type *ptr) noexcept
      : p_(ptr),
        s_(r.s_)
  {
    if (s_)
      s_->get();
  }

  template <typename Y>
  shared_ptr(shared_ptr<Y> &&r, element_type *ptr) noexcept
      : p_(ptr),
        s_(detail::exchange(r.s_, nullptr))
  {
    r.p_ = nullptr;
  }

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

  template <typename Y,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  explicit shared_ptr(const weak_ptr<Y> &r);

  template <typename Y, typename Deleter,
            std::enable_if_t<std::is_convertible_v<Y *, T *>, int> = 0>
  shared_ptr(unique_ptr<Y, Deleter> &&r) : shared_ptr(r.get())
  {
    r.release();
  }

  ~shared_ptr()
  {
    if (s_)
      s_->put();
  }

  shared_ptr &operator=(const shared_ptr &r) noexcept
  {
    shared_ptr(r).swap(*this);
    return *this;
  }

  template <typename Y>
  shared_ptr &operator=(const shared_ptr<Y> &r) noexcept
  {
    shared_ptr(r).swap(*this);
    return *this;
  }

  shared_ptr &operator=(shared_ptr &&r) noexcept
  {
    r.swap(*this);
    return *this;
  }

  template <typename Y>
  shared_ptr &operator=(shared_ptr<Y> &&r) noexcept
  {
    shared_ptr(std::move(r)).swap(*this);
    return *this;
  }

  template <typename Y, typename Deleter>
  shared_ptr &operator=(unique_ptr<Y, Deleter> &&r)
  {
    shared_ptr(std::move(r)).swap(*this);
    return *this;
  }

  void reset() noexcept { shared_ptr().swap(*this); }

  template <typename Y>
  void reset(Y *ptr)
  {
    shared_ptr(ptr).swap(*this);
  }

  template <typename Y, typename Deleter>
  void reset(Y *ptr, Deleter d)
  {
    shared_ptr(ptr, d).swap(*this);
  }

  template <typename Y, typename Deleter, typename Alloc>
  void reset(Y *ptr, Deleter d, Alloc alloc)
  {
    shared_ptr(ptr, std::move(d), alloc).swap(*this);
  }

  void swap(shared_ptr &r) noexcept
  {
    using std::swap;
    swap(p_, r.p_);
    swap(s_, r.s_);
  }

  element_type *get() const noexcept { return p_; }

  template <typename U = T>
  std::enable_if_t<!std::is_void_v<U>, element_type> &operator*() const noexcept
  {
    GPCL_ASSERT(get());
    return *get();
  }

  element_type *operator->() const noexcept
  {
    GPCL_ASSERT(get());
    return get();
  }

  long use_count() const noexcept
  {
    if (s_)
      return s_->use_count();
    return 0;
  }

  explicit operator bool() const noexcept { return get() != nullptr; }

  template <typename Y>
  bool owner_before(const shared_ptr<Y> &other) const noexcept
  {
    return s_ - other.s_ < 0;
  }

  template <typename Y>
  bool owner_before(const weak_ptr<Y> &other) const noexcept;
};

template <typename T, typename Y>
shared_ptr<T> static_pointer_cast(const shared_ptr<Y> &p) noexcept
{
  return shared_ptr<T>(p, static_cast<T *>(p.get()));
}

template <typename T, typename U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U> &p) noexcept
{
  if (auto tp = dynamic_cast<T *>(p.get()))
    return shared_ptr<T>(p, tp);
  return nullptr;
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

} // namespace gpcl

#include <gpcl/allocate_shared.hpp>
#include <gpcl/make_shared.hpp>

#include <gpcl/impl/shared_ptr.hpp>

#endif // GPCL_SHARED_PTR_HPP
