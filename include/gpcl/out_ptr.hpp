//
// out_ptr.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2023-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OUT_PTR_HPP
#define GPCL_OUT_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/out_ptr.hpp>

namespace gpcl {

using detail::out_ptr_t;

template <typename Pointer = void, typename Smart, typename... Args>
auto out_ptr(Smart &s, Args &&...args)
{
  return gpcl::detail::out_ptr<Pointer>(s, std::forward<Args>(args)...);
}

} // namespace gpcl

#endif // GPCL_OUT_PTR_HPP
