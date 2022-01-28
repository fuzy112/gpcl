//
// scoped_array.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SCOPED_ARRAY_HPP
#define GPCL_SCOPED_ARRAY_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/noncopyable.hpp>
#include <gpcl/swap.hpp>

namespace gpcl {
template <typename T>
class scoped_array : noncopyable
{
public:
  using value_type = T;
  using size_type = std::size_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = std::ptrdiff_t;

private:
  T *data_;

public:
  explicit scoped_array(pointer p = nullptr) noexcept : data_(p) {}

  ~scoped_array()
  {
    delete[] data_;
    data_ = nullptr;
  }

  pointer get() noexcept { return data_; }

  const_pointer get() const noexcept { return data_; }

  reference operator[](size_type i) noexcept { return data_[i]; }

  const_reference operator[](size_type i) const noexcept { return data_[i]; }

  void reset(T *p = nullptr) noexcept
  {
    delete[] data_;
    data_ = p;
  }

  void swap(scoped_array &other) noexcept
  {
    using gpcl::swap;
    swap(data_, other.data_);
  }
};

template <typename T>
void swap(scoped_array<T> &a, scoped_array<T> &b) noexcept
{
  a.swap(b);
}

} // namespace gpcl

#endif // GPCL_SCOPED_ARRAY_HPP
