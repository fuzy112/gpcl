//
// allocate_shared.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ALLOCATE_SHARED_HPP
#define GPCL_ALLOCATE_SHARED_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/shared_ptr.hpp>

namespace gpcl {

template <typename T, typename Alloc, typename... Args>
shared_ptr<T> allocate_shared(const Alloc &alloc, Args &&... args)
{
  auto control = detail::shared_block<gpcl::optional<T>, Alloc>::create(
      alloc, in_place, std::forward<Args>(args)...);
  shared_ptr<T> r(detail::create_from_shared_block, control->managed_object(),
                  control);
  detail::shared_ptr_hooks<T>::on_creation(r.get(), r);
  return r;
}

} // namespace gpcl

#endif // GPCL_ALLOCATE_SHARED_HPP
