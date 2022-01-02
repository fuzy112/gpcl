//
// make_shared.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_MAKE_SHARED_HPP
#define GPCL_IMPL_MAKE_SHARED_HPP

#include <gpcl/allocate_shared.hpp>
#include <gpcl/make_shared.hpp>

namespace gpcl {

namespace detail {
template <typename T>
struct make_shared_impl
{
  template <typename... Args>
  auto operator()(Args &&...args) const
      -> std::enable_if_t<!std::is_array_v<T>, shared_ptr<T>>
  {
    return gpcl::allocate_shared<T>(
        std::allocator<T>(), std::forward<Args>(args)...);
  }
};
} // namespace detail

template <typename T>
constexpr detail::make_shared_impl<T> make_shared{};

} // namespace gpcl

#endif // GPCL_IMPL_MAKE_SHARED_HPP
