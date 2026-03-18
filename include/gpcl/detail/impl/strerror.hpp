//
// strerror.ipp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_STRERROR_HPP
#define GPCL_DETAIL_IMPL_STRERROR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/strerror.hpp>
#include <gpcl/detail/throw_system_error.hpp>

#include <cerrno>
#include <string.h>
#include <string>

namespace gpcl::detail {

template <typename StrType>
void strerror_impl(StrType &str, int errnum)
{
#if defined(__STDC_LIB_EXT1__) ||                                              \
    (defined(__STDC_SECURE_LIB__) && defined(_WIN32))
  str.resize(128);
  strerror_s(&str[0], str.size(), errnum);
  str.resize(strlen(str.c_str()));

#elif _POSIX_C_SOURCE >= 200809L && !defined(__EMSCRIPTEN__)
  str = strerror_l(errnum, uselocale((locale_t)0));

#elif (_POSIX_C_SOURCE >= 200112L) && !defined(_GNU_SOURCE)
  str.resize(128);
  int err = strerror_r(errnum, &str[0], str.size());
  if (err > 0)
    GPCL_THROW_ERRNO(err, "strerror_r");
  else
    GPCL_THROW_ERRNO(errno, "strerror_r");
  str.resize(strlen(str.c_str()));

#elif defined(_GNU_SOURCE) && !defined(__EMSCRIPTEN__)
  str.resize(128);
  char *pstr = strerror_r(errnum, &str[0], str.size());
  if (str.c_str() != pstr)
  {
    GPCL_ASSERT(pstr != nullptr);
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

#endif // GPCL_DETAIL_IMPL_STRERROR_HPP
