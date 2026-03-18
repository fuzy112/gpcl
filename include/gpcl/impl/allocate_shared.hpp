//
// allocate_shared.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_ALLOCATE_SHARED_HPP
#define GPCL_IMPL_ALLOCATE_SHARED_HPP

#include <gpcl/allocate_shared.hpp>
#include <gpcl/get_deleter.hpp>
#include <gpcl/shared_ptr.hpp>

namespace gpcl {

namespace detail {

template <typename T>
template <typename Alloc, typename... Args>
auto allocate_shared_impl<T>::operator()(const Alloc &alloc,
                                         Args &&...args) const
    -> std::enable_if_t<!std::is_array_v<T>, shared_ptr<T>>
{
  return shared_ptr<T>(in_place, alloc, std::forward<Args>(args)...);
}

} // namespace detail
} // namespace gpcl

#endif //
