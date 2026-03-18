//
// content_iterator.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CONTENT_ITERATOR_HPP
#define GPCL_CONTENT_ITERATOR_HPP

#include <gpcl/detail/config.hpp>

#include <iterator>

namespace gpcl {

template <typename T>
class content_iterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using reference = T &;

private:
  T content_;

public:
  explicit content_iterator(T x) : content_(x) {}

  const T *operator->() const noexcept { return &content_; }

  const T &operator*() const noexcept { return content_; }

  bool operator==(content_iterator const &other) const noexcept
  {
    return content_ == other.content_;
  }

  bool operator!=(content_iterator const &other) const noexcept
  {
    return !(*this == other);
  }

  content_iterator &operator++()
  {
    content_.next();
    return *this;
  }

  content_iterator operator++(int)
  {
    auto r = *this;
    ++*this;
    return r;
  }
};

} // namespace gpcl

#endif // GPCL_CONTENT_ITERATOR_HPP
