//
// compressed_pointer.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_COMPRESSED_POINTER_HPP
#define GPCL_DETAIL_COMPRESSED_POINTER_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {
namespace detail {

/// Allow storing a bitset in the lowest bits of a pointer.
template <typename T, std::size_t Align = std::alignment_of<T>::value>
class compressed_pointer
{
  std::uintptr_t data_{};

public:
  using element_type = T;
  using pointer = T *;
  using reference = T &;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  static constexpr std::size_t alignment = Align;

  static constexpr std::uintptr_t flag_mask = alignment - 1;

  constexpr explicit compressed_pointer(std::nullptr_t) {}

  explicit compressed_pointer(T *p = nullptr, std::uintptr_t f = 0)
  {
    set_pointer_and_flags(p, f);
  }

  compressed_pointer &operator=(pointer p)
  {
    set_pointer_and_flags(p, flags());
    return *this;
  }

  compressed_pointer &operator=(std::nullptr_t)
  {
    set_pointer_and_flags(0, flags());
    return *this;
  }

  void swap(compressed_pointer &other) noexcept
  {
    using std::swap;
    swap(data_, other.data_);
  }

  pointer get() const { return reinterpret_cast<pointer>(data_ & ~flag_mask); }

  operator pointer() const { return get(); }

  reference operator*() const { return get(); }

  pointer operator->() const { return get(); }

  explicit operator bool() const { return get() != nullptr; }

  std::uintptr_t flags() const { return data_ & flag_mask; }

  void flags(std::uintptr_t f) { set_pointer_and_flags(get(), f); }

  void set_pointer_and_flags(pointer p, std::uintptr_t f)
  {
    GPCL_ASSERT((reinterpret_cast<std::uintptr_t>(p) & flag_mask) == 0);
    GPCL_ASSERT((f & flag_mask) == f);
    data_ = reinterpret_cast<std::uintptr_t>(p) | f;
  }

  template <typename T1, std::size_t Align1>
  friend inline void swap(compressed_pointer<T1, Align1> &x,
                          compressed_pointer<T1, Align1> &y)
  {
    x.swap(y);
  }

  template <typename T1, std::size_t Align1>
  friend inline bool operator==(const compressed_pointer<T1, Align1> &x,
                                std::nullptr_t)
  {
    return x.get() == nullptr;
  }

  template <typename T1, std::size_t Align1>
  friend inline bool operator==(std::nullptr_t,
                                const compressed_pointer<T1, Align1> &x)
  {
    return x.get() == nullptr;
  }

  template <typename T1, std::size_t Align1>
  friend inline bool operator!=(const compressed_pointer<T1, Align1> &x,
                                std::nullptr_t)
  {
    return x.get() != nullptr;
  }

  template <typename T1, std::size_t Align1>
  friend inline bool operator!=(std::nullptr_t,
                                const compressed_pointer<T1, Align1> &x)
  {
    return x.get() != nullptr;
  }

  template <typename T1, std::size_t Align1, typename T2, std::size_t Align2>
  friend inline bool operator==(const compressed_pointer<T1, Align1> &x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x.get() == y.get();
  }

  template <typename T1, std::size_t Align1, typename T2, std::size_t Align2>
  friend inline bool operator!=(const compressed_pointer<T1, Align1> &x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x.get() != y.get();
  }

  template <typename T1, std::size_t Align1, typename T2, std::size_t Align2>
  friend inline bool operator<(const compressed_pointer<T1, Align1> &x,
                               const compressed_pointer<T2, Align2> &y)
  {
    return x.get() < y.get();
  }

  template <typename T1, std::size_t Align1, typename T2, std::size_t Align2>
  friend inline bool operator>(const compressed_pointer<T1, Align1> &x,
                               const compressed_pointer<T2, Align2> &y)
  {
    return x.get() > y.get();
  }

  template <typename T1, std::size_t Align1, typename T2, std::size_t Align2>
  friend inline bool operator<=(const compressed_pointer<T1, Align1> &x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x.get() <= y.get();
  }

  template <typename T1, std::size_t Align1, typename T2, std::size_t Align2>
  friend inline bool operator>=(const compressed_pointer<T1, Align1> &x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x.get() >= y.get();
  }

 template <typename T1, std::size_t Align1, typename T2>
  friend inline bool operator==(const compressed_pointer<T1, Align1> &x,
                                 T2 *y)
  {
    return x.get() == y;
  }

  template <typename T1, std::size_t Align1, typename T2>
  friend inline bool operator!=(const compressed_pointer<T1, Align1> &x,
                                 T2 *y)
  {
    return x.get() != y;
  }


  template <typename T1, std::size_t Align1, typename T2>
  friend inline bool operator<(const compressed_pointer<T1, Align1> &x,
                                T2 *y)
  {
    return x.get() < y;
  }

  template <typename T1, std::size_t Align1, typename T2>
  friend inline bool operator>(const compressed_pointer<T1, Align1> &x,
                                T2 *y)
  {
    return x.get() > y;
  }

  template <typename T1, std::size_t Align1, typename T2>
  friend inline bool operator<=(const compressed_pointer<T1, Align1> &x,
                                 T2 *y)
  {
    return x.get() <= y;
  }

  template <typename T1, std::size_t Align1, typename T2>
  friend inline bool operator>=(const compressed_pointer<T1, Align1> &x,
                                T2 *y)
  {
    return x.get() >= y;
  }

  template <typename T1, typename T2, std::size_t Align2>
  friend inline bool operator==(T1 *x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x == y.get();
  }

  template <typename T1, typename T2, std::size_t Align2>
  friend inline bool operator!=(T1 *x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x != y.get();
  }

  template <typename T1, typename T2, std::size_t Align2>
  friend inline bool operator<(T1 *x,
                               const compressed_pointer<T2, Align2> &y)
  {
    return x < y.get();
  }

  template <typename T1, typename T2, std::size_t Align2>
  friend inline bool operator>(T1 *x,
                               const compressed_pointer<T2, Align2> &y)
  {
    return x > y.get();
  }

  template <typename T1, typename T2, std::size_t Align2>
  friend inline bool operator<=(T1 *x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x <= y.get();
  }

  template <typename T1, typename T2, std::size_t Align2>
  friend inline bool operator>=(T1 *x,
                                const compressed_pointer<T2, Align2> &y)
  {
    return x >= y.get();
  }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_COMPRESSED_POINTER_HPP
