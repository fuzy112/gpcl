//
// get_deleter.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
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
  auto* ty = static_cast<const type_info *>(
      p.s_->operate(detail::ref_count_operation::get_deleter_type_info));
  if (!ty)
    return nullptr;
  if (*ty != typeid_<Deleter>())
    return nullptr;
  return static_cast<Deleter *>(p.s_->operate(detail::ref_count_operation::get_deleter));
}

} // namespace gpcl

#endif // GPCL_IMPL_GET_DELETER_HPP
