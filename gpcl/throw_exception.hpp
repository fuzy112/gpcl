//
// throw_exception.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_THROW_EXCEPTION_HPP
#define GPCL_THROW_EXCEPTION_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/exception.hpp>
#include <gpcl/make_iomanip.hpp>

#ifdef GPCL_NO_EXCEPTIONS
#  include <gpcl/stacktrace.hpp>
#endif

#include <iomanip>

namespace gpcl {

template <typename E>
class wrapped_exception : public E, virtual public exception
{
  static_assert(!std::is_base_of<gpcl::exception, E>::value, "");

public:
  template <typename T>
  explicit wrapped_exception(T &&e) : E(std::forward<T>(e))
  {
  }
};

template <typename E,
          typename std::enable_if<
              std::is_base_of<exception, typename std::decay<E>::type>::value,
              int>::type = 0>
E enable_error_info(E &&e) noexcept
{
  return std::forward<E>(e);
}

template <typename E,
          typename std::enable_if<
              !std::is_base_of<exception, typename std::decay<E>::type>::value,
              int>::type = 0>
wrapped_exception<typename std::decay<E>::type> enable_error_info(E &&e)
{
  return wrapped_exception<typename std::decay<E>::type>(std::forward<E>(e));
}

template <typename E>
[[noreturn]] void throw_exception(E &&e)
{
#if defined GPCL_NO_EXCEPTIONS
  cdebug()
      << "Trying to throw an exception, but exception support is disabled.\n"
      << enable_error_info(e) << "\nTracing back:\n"
      << stacktrace::current() << "\nTerminating..." << std::endl;
#endif
  GPCL_THROW(enable_error_info(std::forward<E>(e)));
}

// Common error infos

namespace detail {
using source_file_errinfo =
    error_info<struct source_file_errinfo_, const char *>;
using source_line_errinfo = error_info<struct source_line_errinfo_, unsigned>;
using func_name_errinfo = error_info<struct func_name_errinfo_, const char *>;

inline decltype(auto) tag_invoke(source_file_errinfo::format_fn,
                                 const char *source_file)
{
  return make_iomanip(
      [=](auto &s) { s << "[source file] = " << std::quoted(source_file); });
}

inline decltype(auto) tag_invoke(source_line_errinfo::format_fn, unsigned line)
{
  return make_iomanip([=](auto &s) { s << "[source line] = " << line; });
}

inline decltype(auto) tag_invoke(func_name_errinfo::format_fn,
                                 const char *func_name)
{
  return make_iomanip([=](auto &s) { s << "[function name] = " << func_name; });
}
} // namespace detail

using function_name_error_info = detail::func_name_errinfo;
#if defined(__GNUC__)
#  define GPCL_FUNCTION_NAME_ERROR_INFO_CURRENT()                              \
    ::gpcl::function_name_error_info(__PRETTY_FUNCTION__)
#else
#  define GPCL_FUNCTION_NAME_ERROR_INFO_CURRENT()                              \
    ::gpcl::function_name_error_info(__func__)
#endif

using source_file_error_info = detail::source_file_errinfo;
#define GPCL_SOURCE_FILE_ERROR_INFO_CURRENT()                                  \
  ::gpcl::source_file_error_info(__FILE__)

using source_line_error_info = detail::source_line_errinfo;
#define GPCL_SOURCE_LINE_ERROR_INFO_CURRENT()                                  \
  ::gpcl::source_line_error_info(__LINE__)

#define GPCL_THROW_EXCEPTION(exc)                                              \
  ::gpcl::throw_exception(::gpcl::enable_error_info(exc)                       \
                          << GPCL_FUNCTION_NAME_ERROR_INFO_CURRENT()           \
                          << GPCL_SOURCE_FILE_ERROR_INFO_CURRENT()             \
                          << GPCL_SOURCE_LINE_ERROR_INFO_CURRENT())

} // namespace gpcl

#endif // GPCL_THROW_EXCEPTION_HPP
