//
// throw_system_error.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_THROW_SYSTEM_ERROR_HPP
#define GPCL_DETAIL_THROW_SYSTEM_ERROR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/source_location.hpp>
#include <gpcl/throw_exception.hpp>

#include <errno.h>

#ifdef GPCL_WINDOWS
#  include <Windows.h>
#endif

namespace gpcl {

namespace detail {

inline namespace errors {

class interrupted : public std::exception
{
public:
  interrupted() = default;
  czstring<> what() const noexcept final { return "thread interrupted"; }
};
} // namespace errors

GPCL_NORETURN inline void throw_system_error(int code,
                                            error_category const &category,
                                            const char *what,
                                            source_location location)
{
  GPCL_THROW(::gpcl::enable_error_info(
                 ::gpcl::detail::system_error(code, category, what))
             << source_location_errinfo(location));
}

#define GPCL_THROW_SYSTEM_ERROR(Code, Category, What)                          \
  ::gpcl::detail::throw_system_error((Code), (Category), (What),               \
                                     GPCL_SOURCE_LOCATION_CURRENT_LINE())

GPCL_NORETURN inline void throw_errno(int error, czstring<> what,
                                     source_location location)
{
  throw_system_error(error, generic_category(), what, location);
}

#define GPCL_THROW_ERRNO(Code, What)                                           \
  ::gpcl::detail::throw_errno((Code), (What),                                  \
                              GPCL_SOURCE_LOCATION_CURRENT_LINE())

#ifdef GPCL_WINDOWS
GPCL_NORETURN inline void throw_last_error(DWORD error, czstring<> what,
                                          source_location location)
{
  throw_system_error(error, system_category(), what, location);
}
#endif

GPCL_NORETURN inline void throw_last_error(czstring<> what,
                                          source_location location)
{
#if defined(GPCL_WINDOWS)
  throw_last_error(::GetLastError(), what, location);
#elif defined(GPCL_POSIX)
  throw_errno(errno, what, location);
#endif
}

template <typename Errc,
          typename std::enable_if<std::is_error_code_enum<Errc>::value,
                                  int>::type = 0>
GPCL_NORETURN inline void throw_system_error(Errc errc, czstring<> what,
                                            source_location location)
{
  GPCL_THROW(enable_error_info(system_error(make_error_code(errc), what))
             << source_location_errinfo(location));
}

GPCL_NORETURN inline void throw_system_error(errc e, czstring<> what,
                                            source_location location)
{
  GPCL_THROW(enable_error_info(system_error(make_error_code(e), what))
             << source_location_errinfo(location));
}

#define GPCL_THROW_LAST_ERROR_IF(...)                                          \
  if ((__VA_ARGS__))                                                           \
  ::gpcl::detail::throw_last_error(GPCL_TO_STR(__VA_ARGS__),                   \
                                   GPCL_SOURCE_LOCATION_CURRENT_LINE())

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_THROW_SYSTEM_ERROR_HPP
