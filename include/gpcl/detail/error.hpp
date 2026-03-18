//
// error.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ERROR_HPP
#define GPCL_DETAIL_ERROR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/zstring.hpp>

#include <iosfwd>

#if !(defined GPCL_CONFIG_NO_EXCEPTIONS)
#  define GPCL_TRY                                                             \
    {                                                                          \
      try
#  define GPCL_CATCH(...) catch (__VA_ARGS__)
#  define GPCL_AND_CATCH(...) GPCL_CATCH(__VA_ARGS__)
#  define GPCL_RETHROW throw
#  define GPCL_CATCH_END }
#  define GPCL_THROW(...) throw __VA_ARGS__

#else
#  define GPCL_TRY if (true)

#  define GPCL_CATCH(...)                                                      \
    if constexpr (false)                                                       \
    (void)[&](__VA_ARGS__)
#  define GPCL_AND_CATCH(...)                                                  \
    ;                                                                          \
    GPCL_CATCH(__VA_ARGS__)
#  define GPCL_RETHROW (void)0
#  define GPCL_CATCH_END ;

#  define GPCL_THROW(...)                                                      \
    do                                                                         \
    {                                                                          \
      (void)sizeof((__VA_ARGS__));                                             \
      ::std::terminate();                                                      \
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
    struct boost::system::is_error_code_enum<enum_type>                        \
        : std::bool_constant<value>                                            \
    {                                                                          \
    };

#  define GPCL_SPECIALIZE_IS_ERROR_CONDITION_ENUM(enum_type, value)            \
    template <>                                                                \
    struct boost::system::is_error_condition_enum<enum_type>                   \
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
    struct std::is_error_code_enum<enum_type> : std::bool_constant<value>      \
    {                                                                          \
    };

#  define GPCL_IS_DECLARE_ERROR_CONDITION_ENUM(enum_type, value)               \
    template <>                                                                \
    struct std::is_error_condition_enum<enum_type> : std::bool_constant<value> \
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

#endif // GPCL_DETAIL_ERROR_HPP
