//
// error.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ERROR_HPP
#define GPCL_DETAIL_ERROR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/zstring.hpp>

#include <iosfwd>

#if !(defined GPCL_NO_EXCEPTIONS)
#  define GPCL_TRY                                                             \
    {                                                                          \
      try
#  define GPCL_CATCH(x) catch (x)
#  define GPCL_RETHROW throw
#  define GPCL_CATCH_END }
#  define GPCL_THROW(x) throw x
#else
#  define GPCL_TRY                                                             \
    {                                                                          \
      if (true)
#  define GPCL_CATCH(x) else
#  define GPCL_RETHROW std::abort()
#  define GPCL_CATCH_END }
#  define GPCL_THROW(x)                                                        \
    do                                                                         \
    {                                                                          \
      (void)x;                                                                 \
      std::abort();                                                            \
    } while (false)
#endif

#if defined(GPCL_WINDOWS)
#  include <windows.h>
#elif defined(GPCL_POSIX)
#  include <cerrno>
#endif

#ifdef GPCL_USE_BOOST_SYSTEM_ERROR

#  include <boost/system/error_code.hpp>
#  include <boost/system/system_error.hpp>

namespace gpcl {
namespace detail {

inline namespace errors {

using system_error = boost::system::system_error;
using error_code = boost::system::error_code;
using boost::system::error_category;
using boost::system::generic_category;
using boost::system::system_category;
using error_condition = boost::system::error_condition;
namespace errc = boost::system::errc;
using errc::make_error_code;
using errc::make_error_condition;

#  define GPCL_SPECIALIZE_IS_ERROR_CODE_ENUM(enum_type, value)                 \
    template <>                                                                \
    struct ::boost::system::is_error_code_enum<enum_type>                      \
        : std::bool_constant<value>                                            \
    {                                                                          \
    };

#  define GPCL_SPECIALIZE_IS_ERROR_CONDITION_ENUM(enum_type, value)            \
    template <>                                                                \
    struct ::boost::system::is_error_condition_enum<enum_type>                 \
        : std::bool_constant<value>                                            \
    {                                                                          \
    };

#  define GPCL_DEFINE_MAKE_ERROR_CODE(enum_type, category)                     \
    namespace boost::system {                                                  \
    inline error_code make_error_code(enum_type e) noexcept                    \
    {                                                                          \
      return error_code(static_cast<int>(e), category);                        \
    }                                                                          \
    }

#  define GPCL_DEFINE_MAKE_ERROR_CONDITION(enum_type, category)                \
    namespace boost::system {                                                  \
    inline error_code make_error_code(enum_type e) noexcept                    \
    {                                                                          \
      return error_condition(static_cast<int>(e), category);                   \
    }                                                                          \
    }

} // namespace errors

} // namespace detail
} // namespace gpcl

#else

#  include <system_error>

namespace gpcl {
namespace detail {

inline namespace errors {

using system_error = std::system_error;
using error_code = std::error_code;
using std::error_category;
using std::generic_category;
using std::make_error_code;
using std::make_error_condition;
using std::system_category;
using errc = std::errc;
using error_condition = std::error_condition;

#  define GPCL_SPECIALIZE_IS_ERROR_CODE_ENUM(enum_type, value)                 \
    template <>                                                                \
    struct ::std::is_error_code_enum<enum_type> : std::bool_constant<value>    \
    {                                                                          \
    };

#  define GPCL_IS_DECLARE_ERROR_CONDITION_ENUM(enum_type, value)               \
    template <>                                                                \
    struct ::std::is_error_condition_enum<enum_type>                           \
        : std::bool_constant<value>                                            \
    {                                                                          \
    };

#  define GPCL_DEFINE_MAKE_ERROR_CODE(enum_type, category)                     \
    namespace std {                                                            \
    inline error_code make_error_code(enum_type e) noexcept                    \
    {                                                                          \
      return error_code(static_cast<int>(e), category);                        \
    }                                                                          \
    }

#  define GPCL_DEFINE_MAKE_ERROR_CONDITION(enum_type, category)                \
    namespace std {                                                            \
    inline error_code make_error_code(enum_type e) noexcept                    \
    {                                                                          \
      return error_condition(static_cast<int>(e), category);                   \
    }                                                                          \
    }

} // namespace errors

} // namespace detail
} // namespace gpcl

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

#if defined(GPCL_POSIX)
[[noreturn]] GPCL_DECL void throw_system_error(int err, czstring<> what);
#elif defined(GPCL_WINDOWS)
[[noreturn]] GPCL_DECL void throw_system_error(DWORD err, czstring<> what);
#endif

[[noreturn]] inline void throw_system_error(czstring<> what)
{
#if defined(GPCL_WINDOWS)
  throw_system_error(GetLastError(), what);
#elif defined(GPCL_POSIX)
  throw_system_error(errno, what);
#endif
}

template <typename Errc, typename std::enable_if<!std::is_integral<Errc>::value,
                                                 int>::type = 0>
[[noreturn]] inline void throw_system_error(Errc errc, czstring<> what)
{
  GPCL_THROW(system_error(make_error_code(errc), what));
}

GPCL_DECL void print_error(int err, czstring<> what) noexcept;
inline void print_error(czstring<> what) noexcept
{
  print_error(errno, what);
}

} // namespace detail
} // namespace gpcl

#if defined(GPCL_HEADER_ONLY)
#  include <gpcl/detail/impl/error.ipp>
#endif

#endif
