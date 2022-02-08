//
// strerror.ipp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_STRERROR_IPP
#define GPCL_DETAIL_IMPL_STRERROR_IPP


#include <gpcl/detail/config.hpp>
#include <gpcl/detail/strerror.hpp>

#include <cerrno>
#include <string.h>
#include <string>

namespace gpcl::detail {

void strerror_impl(std::string &str, int errnum)
{
#if defined(__STDC_LIB_EXT1__) || (defined(__STDC_SECURE_LIB__) && defined(_WIN32))
  str.resize(128);
  strerror_s(&str[0], str.size(), errnum);
  str.resize(strlen(str.c_str()));

#elif _POSIX_C_SOURCE >= 200809L
  static const locale_t loc = uselocale((locale_t)0);
  str = strerror_l(errnum, loc);

#elif defined(_GNU_SOURCE)
  str.resize(1024);
  char *pstr = strerror_r(errnum, &str[0], str.size());
  if (str.c_str() != pstr)
  {
    str = pstr;
  }
  else
  {
    str.resize(strlen(str.c_str()));
  }
#else
  str = std::strerror(errnum);
#endif
}

} // namespace gpcl::detail


#endif // GPCL_DETAIL_IMPL_STRERROR_IPP
