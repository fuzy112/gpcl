//
// allocate_shared.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_ALLOCATE_SHARED_HPP
#define GPCL_IMPL_ALLOCATE_SHARED_HPP

#include <gpcl/allocate_shared.hpp>
#include <gpcl/get_deleter.hpp>
#include <gpcl/shared_ptr.hpp>

namespace gpcl {

namespace detail {
template <typename T>
class sp_inplace_deleter
{
  std::aligned_union_t<sizeof(T), T> storage_;
  bool holds_value_ = false;

public:
  constexpr sp_inplace_deleter() = default;

  /// Copy constructor.
  /// @note It is undefined behaviour to copy a sp_inplace_deleter thats holds
  /// value.
  sp_inplace_deleter(const sp_inplace_deleter &other) noexcept
  {
    GPCL_ASSERT(!other.holds_value_);
  }

  ~sp_inplace_deleter() { destroy(); }

  /// Destroys the held value.
  void destroy() noexcept
  {
    if (holds_value_)
    {
      static_cast<T *>(address())->~T();
      holds_value_ = false;
    }
  }

  /// Destroys the held value.
  void operator()(void *) noexcept { destroy(); }

  /// Returns the address for storing the held value.
  void *address() noexcept { return &storage_; }

  /// Indicates that `this` holds a value.
  void holds_value() noexcept { holds_value_ = true; }
};

} // namespace detail

template <typename T, typename Alloc, typename... Args>
shared_ptr<T> allocate_shared(const Alloc &alloc, Args &&... args)
{
  const shared_ptr<void> x(nullptr, detail::sp_inplace_deleter<T>(), alloc);
  auto *d = gpcl::get_deleter<detail::sp_inplace_deleter<T>>(x);
  GPCL_ASSERT(!!d);
  shared_ptr<T> rv(x, ::new (d->address()) T(std::forward<Args>(args)...),
                   true);
  d->holds_value();
  return rv;
}

} // namespace gpcl

#endif //
