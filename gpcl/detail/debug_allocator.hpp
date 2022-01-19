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

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/scoped_lock.hpp>

#include <cstdlib>
#include <iostream>
#include <map>
#include <typeinfo>

namespace gpcl {
namespace detail {

struct alloc_record
{
  thread_id tid{};

#if !defined GPCL_NO_RTTI
  const std::type_info *type{};
#endif

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

inline debug_allocator_data &g_debug_alloc_data =
    debug_allocator_data::instance();

GPCL_DECL void debug_allocator_double_free(void *p, std::size_t size,
                                           std::size_t count
#if !defined GPCL_NO_RTTI
                                           ,
                                           std::type_info const &type
#endif
);

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
    scoped_lock lock(g_debug_alloc_data.mtx);
    void *p = std::malloc(sizeof(T) * n);
    g_debug_alloc_data.alloc_records_map[p] = alloc_record
    {
      this_thread::id(),
#if !defined GPCL_NO_RTTI
          &typeid(T),
#endif
          sizeof(T), n
    };

    return static_cast<T *>(p);
  }

  void deallocate(T *p, std::size_t n) const
  {
    if (p == 0 || n == 0)
      return;
    scoped_lock lock(g_debug_alloc_data.mtx);

    auto iter = g_debug_alloc_data.alloc_records_map.find(p);
    if (iter == g_debug_alloc_data.alloc_records_map.cend())
    {
      debug_allocator_double_free(p, sizeof(T), n, typeid(T));
    }
    free(p);
    g_debug_alloc_data.alloc_records_map.erase(iter);
  }
};

} // namespace detail
} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/debug_allocator.ipp>
#endif

#endif // GPCL_DETAIL_DEBUG_ALLOCATOR_HPP
