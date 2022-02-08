//
// get_deleter.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_GET_DELETER_HPP
#define GPCL_IMPL_GET_DELETER_HPP

#include <gpcl/get_deleter.hpp>
#include <gpcl/shared_ptr.hpp>

namespace gpcl {

template <typename Deleter, typename T>
Deleter *get_deleter(const shared_ptr<T> &p) noexcept
{
  if (p.use_count() == 0)
    return nullptr;
#if !defined GPCL_NO_RTTI
  auto* ty = static_cast<const type_info *>(
      p.s_->operate(detail::get_deleter_type_info));
  if (!ty)
    return nullptr;
  if (*ty != typeid_<Deleter>())
    return nullptr;
#endif
  return static_cast<Deleter *>(p.s_->operate(detail::get_deleter));
}

} // namespace gpcl

#endif // GPCL_IMPL_GET_DELETER_HPP
