//
// polymorphic_allocator.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_PMR_POLYMORPHIC_ALLOCATOR_HPP
#define GPCL_PMR_POLYMORPHIC_ALLOCATOR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/pmr/default_resource.hpp>
#include <gpcl/pmr/memory_resource.hpp>
#include <limits>

namespace gpcl {
namespace pmr {

/// A polymorphic allocate that allocates memory from a memory resource.
///
/// \remarks polymorphic_allocator does not propagate on container copy
/// assignment, move assignment, or swap. As a result, move assignment of a
/// polymorphic_allocator-using container can throw, and swapping two
/// polymorphic_allocator-using containers whose allocators do not compare equal
/// results in undefined behavior.
template <typename T>
class polymorphic_allocator
{
public:
  using value_type = T;

  /// Constructs a polymorphic_allocator using the return value of
  /// std::pmr::get_default_resource() as the underlying memory resource.
  polymorphic_allocator() noexcept : res_(get_default_resource()) {}

  /// Constructs a polymorphic_allocator using other.resource() as the
  /// underlying memory resource.
  ///
  /// \param other another polymorphic_allocator to copy from.
  ///
  /// Copying a container using a polymorphic_allocator will not call the
  /// allocator's copy constructor. Instead, the new container will use the
  /// return value of select_on_container_copy_construction (a
  /// default-constructed polymorphic_allocator) as its allocator.
  polymorphic_allocator(const polymorphic_allocator &other)
      : polymorphic_allocator(other.resource())
  {
  }

  /// Constructs a polymorphic_allocator using other.resource() as the
  /// underlying memory resource.
  ///
  /// \param other  another polymorphic_allocator to copy from.
  ///
  /// Copying a container using a polymorphic_allocator will not call the
  /// allocator's copy constructor. Instead, the new container will use the
  /// return value of select_on_container_copy_construction (a
  /// default-constructed polymorphic_allocator) as its allocator.
  template <class U>
  polymorphic_allocator(const polymorphic_allocator<U> &other) noexcept
      : polymorphic_allocator(other.resource())
  {
  }

  /// Constructs a polymorphic_allocator using r as the underlying memory
  /// resource. This constructor provides an implicit conversion from
  /// memory_resource*.
  ///
  /// \throws nothing.
  ///
  /// \param r pointer to the memory resource to use. May not be null.
  polymorphic_allocator(memory_resource *r) : res_(r)
  {
    GPCL_ASSERT(r != nullptr);
  }

  /// Copy assignment operator is deleted.
  polymorphic_allocator &operator=(const polymorphic_allocator &) = delete;

  /// Allocates storage for n objects of type T using the underlying memory
  /// resource.
  /// Equivalent to return static_cast<T*>(resource()->allocate(n *
  /// sizeof(T), alignof(T)));
  ///
  /// \throws std::bad_array_new_length if `n >
  /// std::numeric_limits<std::size_t>::max() / sizeof(T)`; may also any
  /// exceptions thrown by the call to resource()->allocate.
  ///
  /// \param n the number of objects to allocate storage for.
  /// \returns A pointer to the allocated storage.
  [[nodiscard]] T *allocate(std::size_t n)
  {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      GPCL_THROW(std::bad_array_new_length());

    return static_cast<T *>(resource()->allocate(n * sizeof(T), alignof(T)));
  }

  /// Deallocates the storage pointed to by p, which must have been allocated
  /// from a pmr::memory_resource x that compares equal to *resource()
  /// using x.allocate(n * sizeof(T), alignof(T)).
  ///
  /// Equivalent to this->resource()->deallocate(p, n * sizeof(T), alignof(T));
  ///
  /// \throws nothing.
  /// \param p pointer to the memory to deallocate.
  /// \param n the number of object originally allocated.
  void deallocate(T *p, std::size_t n)
  {
    this->resource()->deallocate(p, n * sizeof(T), alignof(T));
  }

  /// \returns a default-constructed polymorphic_allocator object.
  /// \remarks polymorphic_allocator do not propagate on container copy
  /// construction.
  polymorphic_allocator select_on_container_copy_construction() const
  {
    return polymorphic_allocator();
  }

  /// \returns the underlying memory resource.
  /// \throws nothing.
  memory_resource *resource() const { return res_; }

  // todo: construct() and destroy()

private:
  memory_resource *res_;
};

/// Compares two polymorphic allocators.
/// Two polymorphic allocators compare
/// equal if their underlying memory resource compares equal.
///
/// \returns `*lhs.resource() == *rhs.resource()`
template <typename T1, typename T2>
bool operator==(const polymorphic_allocator<T1> &lhs,
                const polymorphic_allocator<T2> &rhs) noexcept
{
  return *lhs.resource() == *rhs.resource();
}

/// Compares two polymorphic allocators.
/// Two polymorphic allocators compare
/// equal if their underlying memory resource compares equal.
///
/// \returns `!(lhs == rhs)`
template <typename T1, typename T2>
bool operator!=(const polymorphic_allocator<T1> &lhs,
                const polymorphic_allocator<T2> &rhs) noexcept
{
  return !(lhs == rhs);
}

} // namespace pmr
} // namespace gpcl

#endif // GPCL_PMR_POLYMORPHIC_ALLOCATOR_HPP
