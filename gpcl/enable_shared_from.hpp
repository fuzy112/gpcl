//
// enable_shared_from.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ENABLE_SHARED_FROM_HPP
#define GPCL_ENABLE_SHARED_FROM_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/enable_shared_from_this.hpp>

namespace gpcl {

class enable_shared_from
#ifndef GPCL_DOXYGEN
    : public enable_shared_from_this<enable_shared_from>
#endif
{
protected:
  ~enable_shared_from() = default;
};

/// @relates gpcl::enable_shared_from
/// @tparam T should inherit enable_shared_from.
template <class T>
shared_ptr<T> shared_from(T *p)
{
  return static_pointer_cast<T>(p->enable_shared_from::shared_from_this());
}

/// @relates gpcl::enable_shared_from
/// @tparam T should inherit enable_shared_from.
template <class T>
weak_ptr<T> weak_from(T *p) noexcept
{
  return static_pointer_cast<T>(p->enable_shared_from::weak_from_this());
}

} // namespace gpcl

#endif // GPCL_ENABLE_SHARED_FROM_HPP
