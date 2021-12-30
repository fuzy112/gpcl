//
// pool.hpp
// ~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_POOL_HPP
#define GPCL_POOL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/simple_segregated_storage.hpp>
#include <gpcl/unique_lock.hpp>

#include <cstddef>

namespace gpcl {

#ifndef GPCL_DOXYGEN
template <typename T, typename = void>
struct is_user_allocator : std::false_type
{
};

template <typename T>
struct is_user_allocator<
    T, detail::void_t<decltype(T::malloc(0)), decltype(T::free((char *)0)),
                      typename T::size_type, typename T::difference_type>>
    : std::true_type
{
};
#endif

struct default_malloc_free_user_allocator
{
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  [[nodiscard]] static char *malloc(size_type sz) noexcept
  {
    return static_cast<char *>(std::malloc(sz));
  }

  static void free(char *p) noexcept { std::free(p); }
};

struct default_new_delete_user_allocator
{
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  [[nodiscard]] static char *malloc(size_type sz) noexcept
  {
    return reinterpret_cast<char *>(::operator new[](sz, std::nothrow));
  }

  static void free(char *p) noexcept { ::operator delete[](p, std::nothrow); }
};

template <typename UA = default_new_delete_user_allocator,
          typename MutexType = gpcl::mutex>
class pool
{
  static_assert(is_user_allocator<UA>::value, "user allocator");

public:
  using user_allocator_type = UA;
  using mutex_type = MutexType;
  using size_type = typename user_allocator_type::size_type;
  using difference_type = typename user_allocator_type::difference_type;

  /// Constructor.
  explicit pool(size_type requested_size)
      : requested_size_(requested_size),
        chunk_size_(std::lcm(requested_size_,
                             std::lcm(sizeof(void *), sizeof(size_type)))),
        block_list_(),
        next_size_(32)
  {
    GPCL_ASSERT(requested_size != 0);
  }

  /// Destructor.
  ///
  /// During destruction, any malloc'd but not free'd memory will be recycled.
  ~pool() noexcept
  {
    block_info block = block_list_;
    while (block.ptr)
    {
      auto next = *reinterpret_cast<block_info *>(
          block.ptr + block_info_offset(block.size));
      user_allocator_type::free(block.ptr);
      block = next;
    }
  }

  /// Determines the requested size.
  size_type requested_size() const noexcept { return requested_size_; }

  /// Allocates memory for an object of size requested_size.
  [[nodiscard]] void *malloc()
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);

    if (storage_.empty())
    {
      if (request_new_block(chunk_size_))
        return storage_.malloc(chunk_size_);
      else
        return nullptr;
    }

    return storage_.malloc(chunk_size_);
  }

  /// Allocates memory for an object of size requested_size.
  [[nodiscard]] void *ordered_malloc()
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);

    if (storage_.empty())
    {
      if (request_new_block(chunk_size_, true))
        return storage_.malloc(chunk_size_);
      else
        return nullptr;
    }

    return storage_.malloc(chunk_size_);
  }

  /// Allocates memory for an array of n objects of size requested_size.
  [[nodiscard]] void *ordered_malloc(size_type n)
  {
    GPCL_ASSERT(n != 0);
    auto count = chunk_count(n);
    gpcl::unique_lock<mutex_type> lock(mutex_);
    if (auto ret = storage_.malloc_n(count, chunk_size_))
      return ret;
    request_new_block(count * chunk_size_, true);

    return storage_.malloc_n(count, chunk_size_);
  }

  /// Frees memory for an object.
  void free(void *ptr)
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);
    storage_.free(ptr);
  }

  /// Frees memory for an object.
  void ordered_free(void *ptr)
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);
    storage_.ordered_free(ptr);
  }

  /// Frees memory for an array of objects.
  void ordered_free(void *ptr, size_type n)
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);
    storage_.ordered_free_n(ptr, chunk_count(n), chunk_size_);
  }

  // not thread-safe
  size_type get_next_size() const { return next_size_; }

  // not thread-safe
  void set_next_size(size_type next_size)
  {
    GPCL_ASSERT(next_size != 0);
    next_size_ = next_size;
  }

  bool is_from(void *) const noexcept
  {
    GPCL_UNIMPLEMENTED();
    return false;
  }

private:
  struct block_info
  {
    // Pointer to the start of the next block.
    char *ptr;

    // Size of the next block.
    size_type size;
  }
#ifdef __GNUC__
  __attribute__((may_alias))
#endif
  ;

  // Requests a block with at least min_size bytes for chunks.
  bool request_new_block(size_type min_size, bool ordered = false)
  {
    auto sz = next_size_ * chunk_size_;
    while (sz < min_size + sizeof(block_info) * 2)
      sz *= 2;

    next_size_ *= 2;

    char *block = user_allocator_type::malloc(sz);
    if (block == nullptr)
      return false;

    block_list_ = add_block(block, sz, block_list_, ordered);
    GPCL_VERIFY_FALSE(storage_.empty());
    return true;
  }

  block_info add_block(char *ptr, size_type sz, block_info end, bool ordered)
  {
    auto chunks_size = block_info_offset(sz);
    if (ordered)
      storage_.add_ordered_block(ptr, chunks_size, chunk_size_);
    else
      storage_.add_block(ptr, chunks_size, chunk_size_);
    *reinterpret_cast<block_info *>(ptr + chunks_size) = end;
    return {ptr, sz};
  }

  difference_type block_info_offset(size_type block_size)
  {
    return (block_size - sizeof(block_info)) / alignof(block_info) *
           alignof(block_info);
  }

  size_type chunk_count(size_type n)
  {
    const size_type array_size = n * requested_size_;
    const size_type ret = (array_size + chunk_size_ - 1) / chunk_size_;
    GPCL_VERIFY(ret * chunk_size_ >= n * requested_size_);
    return ret;
  }

  mutable mutex_type mutex_;
  const size_type requested_size_;
  const size_type chunk_size_;
  size_type next_size_;
  block_info block_list_;
  simple_segregated_storage<size_type> storage_;
};

template <typename Tag, unsigned RequestedSize,
          typename UserAllocator = default_malloc_free_user_allocator>
class singleton_pool
{
  using pool_type = pool<UserAllocator>;

public:
  using tag = Tag;
  using user_allocator = UserAllocator;
  using size_type = typename pool<UserAllocator>::size_type;
  using difference_type = typename pool<UserAllocator>::difference_type;

  const static unsigned requested_size = RequestedSize;

  static void *malloc() { return get_pool().malloc(); }

  static void *ordered_malloc() { return get_pool().ordered_malloc(); }

  static void *ordered_malloc(size_type n)
  {
    return get_pool().ordered_malloc(n);
  }

  static void free(void *ptr) { get_pool().free(ptr); }

  static void ordered_free(void *ptr) { get_pool().ordered_free(ptr); }

  static void ordered_free(void *ptr, size_type n)
  {
    get_pool().ordered_free(ptr, n);
  }

private:
  static pool_type &get_pool()
  {
    static pool_type instance(RequestedSize);
    return instance;
  }
};

} // namespace gpcl

#endif // GPCL_POOL_HPP
