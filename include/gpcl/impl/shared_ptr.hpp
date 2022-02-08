//
// shared_ptr.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_SHARED_PTR_HPP
#define GPCL_IMPL_SHARED_PTR_HPP

#include <gpcl/bad_weak_ptr.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/enable_shared_from_this.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/weak_ptr.hpp>

namespace gpcl {

template <typename T>
template <typename Y, std::enable_if_t<std::is_convertible_v<Y *, T *>, int>>
shared_ptr<T>::shared_ptr(const weak_ptr<Y> &r)
{
  r.lock().swap(*this);
  if (!s_)
    GPCL_THROW(bad_weak_ptr());
}

template <typename T>
template <typename Y>
bool shared_ptr<T>::owner_before(const weak_ptr<Y> &other) const noexcept
{
  return s_ - other.s_ < 0;
}

template <typename T>
template <typename Y>
void shared_ptr<T>::enables_shared_from_this(Y *ptr) noexcept
{
  if constexpr (std::is_base_of_v<detail::enable_shared_from_this_base,
                                  std::remove_cv_t<Y>>)
  {
    if (ptr != nullptr && ptr->weak_this.expired())
      ptr->weak_this = shared_ptr<std::remove_cv_t<Y>>(
          *this, const_cast<std::remove_cv_t<Y> *>(ptr));
  }
  else
  {
    (void)ptr;
  }
}

} // namespace gpcl

#endif // GPCL_IMPL_SHARED_PTR_HPP
