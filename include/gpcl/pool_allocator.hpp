//
// pool_allocator.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_POOL_ALLOCATOR_HPP
#define GPCL_POOL_ALLOCATOR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/pool.hpp>

namespace gpcl {

/// Allocator that allocates from a global pool.
template <typename T>
class pool_allocator
{
  using pool_type = singleton_pool<class unordered_tag, sizeof(T)>;
  using ordered_pool_type = singleton_pool<class ordered_tag, sizeof(T)>;

public:
  using value_type = T;
  using size_type = typename pool_type::size_type;
  using difference_type = typename pool_type::size_type;

  pool_allocator() = default;

  template <typename U>
  explicit pool_allocator(const pool_allocator<U> &) noexcept
  {
  }

  T *allocate(size_type n)
  {
    if (n != 1)
      return (T *)ordered_pool_type::ordered_malloc(n);
    else
      return (T *)pool_type::malloc();
  }

  void deallocate(T *ptr, size_type n)
  {
    if (n != 1)
      ordered_pool_type::ordered_free(ptr, n);
    else
      pool_type::free(ptr);
  }
};

} // namespace gpcl

#endif // GPCL_POOL_ALLOCATOR_HPP
