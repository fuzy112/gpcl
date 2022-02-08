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

#include <string>

namespace gpcl::detail {

GPCL_DECL
void strerror_impl(std::string &str, int errnum);
} // namespace gpcl::detail
  


#endif // GPCL_DETAIL_STRERROR_HPP
