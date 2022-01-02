//
// make_shared.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MAKE_SHARED_HPP
#define GPCL_MAKE_SHARED_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {

template <typename T>
class shared_ptr;

/** @addtogroup smart_pointer Smart Pointers
 *  @{
 */

#if defined GPCL_DOXYGEN
/// Creates a shared pointer that manages a new object.
/**
 * @param args list of arguments with which an instance of T will be
 * constructed.
 * @return shared_ptr of an instance of type T.
 *
 * @exception std::bad_alloc if failed to allocate memory.
 * @exception any-exception thrown by the constructor.
 *
 * If any exception is thrown, the function has no effect.
 *
 * @relates gpcl::shared_ptr
 */
template <typename T, typename... Args>
auto make_shared(Args &&...args)
    -> std::enable_if_t<!std::is_array_v<T>, shared_ptr<T>>;

#else
namespace detail {
template <typename T>
struct make_shared_impl
{
  template <typename... Args>
  auto operator()(Args &&...args) const
      -> std::enable_if_t<!std::is_array_v<T>, shared_ptr<T>>;
};
} // namespace detail

template <typename T>
constexpr detail::make_shared_impl<T> make_shared{};

#endif

/** @} */

} // namespace gpcl

#include <gpcl/impl/make_shared.hpp>

#endif // GPCL_MAKE_SHARED_HPP
