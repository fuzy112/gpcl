//
// debug_allocator.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_DEBUG_ALLOCATOR_HPP
#define GPCL_DETAIL_DEBUG_ALLOCATOR_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/scoped_lock.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/typeid.hpp>

#include <cstdlib>
#include <iostream>
#include <map>

namespace gpcl {
namespace detail {

struct alloc_record
{
  thread_id tid{};

  const type_info *type{};

  std::size_t size{};
  std::size_t count{};
};

struct debug_allocator_data
{
  recursive_mutex mtx;
  std::map<const void *, alloc_record> alloc_records_map;

private:
  GPCL_DECL debug_allocator_data();
  GPCL_DECL ~debug_allocator_data();

public:
  static GPCL_DECL debug_allocator_data &instance();
};

inline auto &g_debug_alloc_data = debug_allocator_data::instance();

template <typename T>
class debug_allocator
{
public:
  using value_type = T;

  debug_allocator() = default;

  debug_allocator(const debug_allocator &) = default;

  template <typename U>
  debug_allocator(const debug_allocator<U> &)
  {
  }

  debug_allocator &operator=(const debug_allocator &) = default;

  template <typename U>
  debug_allocator &operator=(const debug_allocator<U> &)
  {
    return *this;
  }

  constexpr bool operator==(const debug_allocator &) const { return true; }

  constexpr bool operator!=(const debug_allocator &) const { return false; }

  T *allocate(std::size_t n) const
  {
    if (n == 0)
      return nullptr;
    scoped_lock lock(g_debug_alloc_data.mtx);
    void *p = std::malloc(sizeof(T) * n);
    GPCL_ASSERT(p != nullptr);
    g_debug_alloc_data.alloc_records_map[p] = alloc_record{
        this_thread::get_id(),
        &typeid_<T>(),
        sizeof(T),
        n,
    };

    return static_cast<T *>(p);
  }

  void deallocate(T *p, std::size_t n) const
  {
    if (p == 0 && n == 0)
      return;
    scoped_lock lock(g_debug_alloc_data.mtx);

    auto iter = g_debug_alloc_data.alloc_records_map.find(p);
    GPCL_ASSERT(iter != g_debug_alloc_data.alloc_records_map.cend());
    std::free(p);
    if (iter != g_debug_alloc_data.alloc_records_map.cend())
      g_debug_alloc_data.alloc_records_map.erase(iter);
  }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_DEBUG_ALLOCATOR_HPP
