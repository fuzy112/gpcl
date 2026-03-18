//
// strerror.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_STRERROR_HPP
#define GPCL_DETAIL_STRERROR_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {
namespace detail {

template <typename StrType>
void strerror_impl(StrType &str, int errnum);

template <typename T>
void strerror_impl(const T &, int) = delete;

} // namespace detail
} // namespace gpcl

#include <gpcl/detail/impl/strerror.hpp>

#endif // GPCL_DETAIL_STRERROR_HPP
