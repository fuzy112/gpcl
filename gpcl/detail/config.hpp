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

#if !defined(GPCL_HEADER_ONLY) & !defined(GPCL_STANDARDESE)
#  if !defined(GPCL_SEPARATE_COMPILATION)
#    if !defined(GPCL_DYN_LINK)
#      define GPCL_HEADER_ONLY 1
#    endif
#  endif
#endif

#if defined(GPCL_HEADER_ONLY)
#  define GPCL_DECL inline
#elif defined(GPCL_DYN_LINK)
#  if defined(_MSC_VER)
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

#ifdef _WIN32
#  define GPCL_WINDOWS
#endif

#ifdef __COBALT__
#  define GPCL_XENOMAI
#endif

#ifdef __linux__
#  define GPCL_POSIX
#endif

#ifdef GPCL_WINDOWS
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#endif

#ifdef GPCL_ENABLE_BOOST
#  define GPCL_USE_BOOST_SYSTEM_ERROR
#  define GPCL_USE_BOOST_CHRONO
#  define GPCL_USE_BOOST_ASSERT
#endif

#ifndef GPCL_CXX17_INLINE_CONSTEXPR
#  if __cplusplus < 201703
#    define GPCL_CXX17_INLINE_CONSTEXPR constexpr
#  else
#    define GPCL_CXX17_INLINE_CONSTEXPR inline constexpr
#  endif
#endif

#ifndef GPCL_CXX17_IF_CONSTEXPR
#  if __cplusplus < 201703
#    define GPCL_CXX17_IF_CONSTEXPR if
#  else
#    define GPCL_CXX17_IF_CONSTEXPR if constexpr
#  endif
#endif

#if __cpp_exceptions
#  undef GPCL_NO_EXCEPTIONS
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

// #define GPCL_NO_RTTI 1
// #undef GPCL_NO_RTTI

/// Main namespace of GPCL
namespace gpcl {

/// Implementation details
namespace detail {
}

} // namespace gpcl

#endif
