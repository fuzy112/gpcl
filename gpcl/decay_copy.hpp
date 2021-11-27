//
// decay_copy.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DECAY_COPY_HPP
#define GPCL_DECAY_COPY_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>

namespace gpcl {

template <typename T>
std::decay_t<T> decay_copy(T &&v)
{
  return detail::forward<T>(v);
}

} // namespace gpcl

#endif // GPCL_DECAY_COPY_HPP
