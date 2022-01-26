//
// throw_system_error.ipp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_THROW_SYSTEM_ERROR_IPP
#define GPCL_DETAIL_IMPL_THROW_SYSTEM_ERROR_IPP


#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/exception.hpp>

namespace gpcl {

template GPCL_NORETURN_UNLESS_CLANG void
throw_exception(detail::system_error &&e);
template GPCL_NORETURN_UNLESS_CLANG void
throw_exception(detail::system_error &e);

} // namespace gpcl

namespace gpcl::detail {

#if defined(GPCL_POSIX)
void throw_system_error(int err, czstring<> what)
{
  if (err == EINTR)
  {
    GPCL_THROW(interrupted());
  }

  throw_exception(system_error(err, generic_category(), what));
}

#elif defined(GPCL_WINDOWS)
[[noreturn]] void throw_system_error(DWORD err, czstring<> what)
{
  throw_exception(system_error(err, system_category(), what));
}
#endif

} // namespace gpcl::detail


#endif // GPCL_DETAIL_IMPL_THROW_SYSTEM_ERROR_IPP
