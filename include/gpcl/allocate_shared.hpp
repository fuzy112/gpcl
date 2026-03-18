//
// allocate_shared.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ALLOCATE_SHARED_HPP
#define GPCL_ALLOCATE_SHARED_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {

template <typename T>
class shared_ptr;

/** @addtogroup smart_pointer
 *  @{
 */

#if defined GPCL_DOXYGEN
/// Creates a shared pointer that manages a new object.
/**
 * @tparam T the managed object type.
 * @tparam Alloc the allocator type.
 *
 * @param alloc the allocator used to allocate memory for the object and for
 * internal use.
 * @param args list of arguments with which an instance of T will be
 * constructed.
 * @return shared_ptr of an instance of type T.
 *
 * @exception std::bad_alloc if failed to allocate memory.
 * 
 * If any exception is thrown, the function has no effect.
 *
 * @relates gpcl::shared_ptr
 */
template <typename T, typename Alloc, typename... Args>
shared_ptr<T> allocate_shared(const Alloc &alloc, Args &&...args);

#else

namespace detail {
template <typename T>
struct allocate_shared_impl
{
  template <typename Alloc, typename... Args>
  auto operator()(const Alloc &alloc, Args &&...args) const
      -> std::enable_if_t<!std::is_array_v<T>, shared_ptr<T>>;
};

} // namespace detail

template <typename T>
constexpr detail::allocate_shared_impl<T> allocate_shared{};

#endif

/** @} */

} // namespace gpcl

#include <gpcl/impl/allocate_shared.hpp>

#endif // GPCL_ALLOCATE_SHARED_HPP
