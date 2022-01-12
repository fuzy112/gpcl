#ifndef GPCL_DETAIL_DEBUG_ALLOCATOR_HPP
#define GPCL_DETAIL_DEBUG_ALLOCATOR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/unique_lock.hpp>

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

  ~debug_allocator_data()
  {
    if (!alloc_records_map.empty())
    {
      std::cerr << "memory leak detected!\n";

      for (auto &&[address, record] : alloc_records_map)
      {
        std::cerr << "address " << address << ": "
                  << "size: " << record.size << ", "
                  << "count: " << record.count << ", "
                  << "total bytes: " << record.size * record.count << ", "
                  << "type: " << record.type->name() << '\n';
      }

      std::abort();
    }
    else
    {
      std::cerr << "no memory leak!\n";
    }
  }
};

inline debug_allocator_data g_debug_alloc_data;

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
    unique_lock lock(g_debug_alloc_data.mtx);
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
    unique_lock lock(g_debug_alloc_data.mtx);

    auto iter = g_debug_alloc_data.alloc_records_map.find(p);
    if (iter == g_debug_alloc_data.alloc_records_map.cend())
    {
      std::cerr << "double free!\n";
      std::cerr << "address " << p << ","
                << "size: " << sizeof(T) << ", "
                << "count: " << n << ", "
                << "total bytes: " << sizeof(T) * n << ","
                << "type: " << typeid(T).name() << '\n';
      std::abort();
      return;
    }
    free(p);
    g_debug_alloc_data.alloc_records_map.erase(iter);
  }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_DEBUG_ALLOCATOR_HPP
