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
#include <gpcl/source_location.hpp>

#ifdef GPCL_CONFIG_NO_EXCEPTIONS
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
#if defined GPCL_CONFIG_NO_EXCEPTIONS
  cdebug()
      << "Trying to throw an exception, but exception support is disabled.\n"
      << enable_error_info(e) << "\nTracing back:\n"
      << stacktrace::current() << "\nTerminating..." << std::endl;
#endif
  GPCL_THROW(enable_error_info(std::forward<E>(e)));
}

// Common error infos

namespace detail {
using source_location_errinfo =
    gpcl::error_info<struct source_location_errinfo_, source_location>;

inline auto tag_invoke(source_location_errinfo::format_fn,
                       source_location location)
{
  return make_iomanip([location](auto &stream) {
    stream << "[source_location_errinfo] = {" << location << "}";
  });
}

} // namespace detail

using source_location_errinfo = detail::source_location_errinfo;
#define GPCL_THROW_EXCEPTION(exc)                                              \
  ::gpcl::throw_exception(                                                     \
      ::gpcl::enable_error_info(exc)                                           \
      << ::gpcl::source_location_errinfo(GPCL_SOURCE_LOCATION_CURRENT_LINE()))

} // namespace gpcl

#endif // GPCL_THROW_EXCEPTION_HPP
