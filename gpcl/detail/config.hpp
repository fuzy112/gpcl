//
// config.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_CONFIG_HPP
#define GPCL_DETAIL_CONFIG_HPP

#include <gpcl/version.hpp>

#if defined(GPCL_DYN_LINK)
#  if defined(_MSC_VER) || defined(__MINGW64__) || defined(__MING32__)
#    if defined(GPCL_SOURCE)
#      define GPCL_DECL __declspec(dllexport)
#    else
#      define GPCL_DECL __declspec(dllimport)
#    endif
#  elif defined(__GNUC__)
#    define GPCL_DECL __attribute__((visibility("default")))
#  else
#    define GPCL_DECL
#  endif
#else
#  define GPCL_DECL
#endif

#if defined(GPCL_DYN_LINK) && defined(GPCL_SOURCE)
#  define GPCL_EXPORT_DECL GPCL_DECL
#else
#  define GPCL_EXPORT_DECL
#endif

#define __STDC_WANT_LIB_EXT1__ 1
#define __STDC_WANT_SECURE_LIB__ 1

#ifdef _WIN32
#  define GPCL_WINDOWS
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define GPCL_UNIX
#endif

#if defined(GPCL_UNIX)
#  include <unistd.h>
#endif

#ifdef __linux__
#  define GPCL_LINUX
#endif

#ifdef _POSIX_VERSION
#  define GPCL_POSIX
#endif

#ifdef GPCL_WINDOWS
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT _WIN32_WINNT_WINBLUE
#  endif
#  ifndef NTDDI_VERSION
#    define NTDDI_VERSION NTDDI_WINBLUE
#  endif
#endif

#ifdef GPCL_ENABLE_BOOST
#  define GPCL_USE_BOOST_SYSTEM_ERROR
#  define GPCL_USE_BOOST_CHRONO
#  define GPCL_USE_BOOST_ASSERT
#endif

#if __cplusplus < 201100 && !defined _MSC_VER
#  error "C++11 or better is required"
#endif

#ifdef __has_include
#  if __has_include(<version>)
#    include <version>
#  endif
#endif

#ifndef GPCL_CXX17_IF_CONSTEXPR
#  ifdef __cpp_if_constexpr
#    define GPCL_CXX17_IF_CONSTEXPR if constexpr
#  else
#    define GPCL_CXX17_IF_CONSTEXPR if
#  endif
#endif

#if __cpp_exceptions
// #  undef GPCL_NO_EXCEPTIONS
#else
#  define GPCL_NO_EXCEPTIONS 1
#endif

#ifndef GPCL_FORCE_INLINE
#  if defined(_MSC_VER)
#    define GPCL_FORCE_INLINE __force_inline
#  elif defined(__GNUC__)
#    define GPCL_FORCE_INLINE __attribute__((always_inline))
#  else
#    define GPCL_FORCE_INLINE inline
#  endif
#endif

#ifndef GPCL_CONFIG_GETOPT_LONG_OPT_EQUAL_SIZE
#  define GPCL_CONFIG_GETOPT_LONG_OPT_EQUAL_SIZE 1
#endif

#if __cpp_rtti
// #  undef GPCL_NO_RTTI
#else
#  define GPCL_NO_RTTI 1
#endif

// #undef GPCL_DISABLE_AUTO_LINKING

#if defined _MSC_VER && defined GPCL_SEPARATE_COMPILATION &&                   \
    !defined GPCL_SOURCE && !defined GPCL_DISABLE_AUTO_LINKING
#  pragma comment(lib, "gpcl")
#endif

/// Main namespace of GPCL
namespace gpcl {

} // namespace gpcl

#endif // GPCL_DETAIL_CONFIG_HPP
