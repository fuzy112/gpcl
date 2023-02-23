//
// pointer_traits.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2023 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_POINTER_TRAITS_HPP
#define GPCL_POINTER_TRAITS_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

template <typename Pointer>
struct pointer_traits
{
};

template <typename T>
struct pointer_traits<T *>
{
  typedef T *pointer;
  typedef T element_type;
  typedef std::ptrdiff_t difference_type;

  template <typename U>
  using rebind = U *;

  static pointer pointer_to(T &x) noexcept { return std::addressof(x); }
};

} // namespace gpcl

#endif // GPCL_POINTER_TRAITS_HPP
