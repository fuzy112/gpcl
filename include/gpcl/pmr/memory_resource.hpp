//
// memory_resource.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_MEMORY_RESOURCE_HPP
#define GPCL_PMR_MEMORY_RESOURCE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>

#include <cstddef> // for max_align_t
#include <cstdint>
#include <cstring>

namespace gpcl {
namespace pmr {

/// The class std::pmr::memory_resource is an abstract interface to an unbounded
/// set of classes encapsulating memory resources.
class memory_resource
{
public:
  /// Destructor.
  virtual ~memory_resource() = default;

  /// \effects Allocates storage with a size of at least bytes bytes. The
  /// returned storage is aligned to the specified alignment if such alignment
  /// is supported, and to alignof(std::max_align_t) otherwise.
  ///
  /// \returns `do_allocate(bytes, alignment)`.
  ///
  /// \throws Throws an exceptions if storage of the requested size and
  /// alignment cannot be obtained.
  [[nodiscard]] void *
  allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    void *ret = do_allocate(bytes, alignment);
    GPCL_ASSERT(reinterpret_cast<std::uintptr_t>(ret) % alignment == 0);
    return ret;
  }

  /// Deallocates the storage pointed to by p. p shall have been returned by a
  /// prior call to allocate(bytes, alignment) on a memory_resource that
  /// compares equal to *this, and the storage it points to shall not yet have
  /// been deallocated.
  ///
  /// Equivalent to do_deallocate(p, bytes, alignment);
  ///
  /// \throws nothing.
  void deallocate(void *p, std::size_t bytes,
                  std::size_t alignment = alignof(std::max_align_t))
  {
    GPCL_ASSERT(reinterpret_cast<std::uintptr_t>(p) % alignment == 0);
    return do_deallocate(p, bytes, alignment);
  }

  /// Compares *this for equality with other. Two memory_resources compare equal
  /// if and only if memory allocated from one memory_resource can be
  /// deallocated from the other and vice versa.
  ///
  /// \returns `do_is_equal(other)`.
  bool is_equal(const memory_resource &other) const noexcept
  {
    return do_is_equal(other);
  }

private:
  /// \throws Allocates storage with a size of at least bytes bytes, aligned to
  /// the specified alignment.
  ///
  /// \requires alignment shall be a power of two.
  /// \throws an exception if storage of the requested size and alignment cannot
  /// be obtained.
  virtual void *do_allocate(std::size_t bytes, std::size_t alignment) = 0;

  /// Deallocates the storage pointed to by p.
  ///
  /// \requires p must have been returned by a prior call to allocate(bytes,
  /// alignment) on a memory_resource that compares equal to *this, and the
  /// storage it points to must not yet have been deallocated, otherwise the
  /// behavior is undefined.
  /// \throws nothing.
  virtual void do_deallocate(void *p, std::size_t bytes,
                             std::size_t alignment) = 0;

  /// Compares *this for equality with other.
  ///
  /// Two memory_resources compare equal if and only if memory allocated from
  /// one memory_resource can be deallocated from the other and vice versa.
  ///
  /// \remarks The most-derived type of other may not match the most derived
  /// type of *this. A derived class implementation therefore must typically
  /// check whether the most derived types of *this and other match using
  /// dynamic_cast, and immediately return false if the cast fails.
  virtual bool do_is_equal(const memory_resource &other) const noexcept = 0;
};

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_MEMORY_RESOURCE_HPP
