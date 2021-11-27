//
// weak_ptr.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_WEAK_PTR_HPP
#define GPCL_IMPL_WEAK_PTR_HPP

#include <gpcl/shared_ptr.hpp>
#include <gpcl/weak_ptr.hpp>

namespace gpcl {

template <typename T>
template <typename Y, std::enable_if_t<std::is_convertible_v<Y *, T *>, int>>
weak_ptr<T>::weak_ptr(const shared_ptr<Y> &r) noexcept : p_(r.p_),
                                                         s_(r.s_)
{
  if (s_)
    s_->weak_get();
}

template <typename T>
template <typename Y, std::enable_if_t<std::is_convertible_v<Y *, T *>, int>>
weak_ptr<T> &weak_ptr<T>::operator=(const shared_ptr<Y> &r) noexcept
{
  weak_ptr(r).swap(*this);
  return *this;
}

template <typename T>
template <typename Y>
bool weak_ptr<T>::owner_before(const shared_ptr<Y> &other) const noexcept
{
  return s_ < other.s_;
}

} // namespace gpcl

#endif // GPCL_IMPL_WEAK_PTR_HPP
