//
// strerror.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_STRERROR_HPP
#define GPCL_DETAIL_STRERROR_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl::detail {

template <typename StrType>
void strerror_impl(StrType &str, int errnum);
} // namespace gpcl::detail

#include <gpcl/detail/impl/strerror.hpp>

#endif // GPCL_DETAIL_STRERROR_HPP
