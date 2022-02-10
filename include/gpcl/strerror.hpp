//
// strerror.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_STRERROR_HPP
#define GPCL_STRERROR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/strerror.hpp>

namespace gpcl {
inline std::string strerror(int errnum)
{
  std::string str;
  detail::strerror_impl(str, errnum);
  return str;
}

template <typename Allocator>
inline void
strerror(std::basic_string<char, std::char_traits<char>, Allocator> &str,
         int errnum)
{
  detail::strerror_impl(str, errnum);
}
} // namespace gpcl

#endif // GPCL_STRERROR_HPP
