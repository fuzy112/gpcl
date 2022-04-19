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

// =================================================================================================
//  COMPILER VERSION
// =============================================================================

// ideas for the version stuff are taken from here:
// https://github.com/cxxstuff/cxx_detect

#define GPCL_COMPILER(MAJOR, MINOR, PATCH)                                     \
  ((MAJOR)*10000000 + (MINOR)*100000 + (PATCH))

// GCC/Clang and GCC/MSVC are mutually exclusive, but Clang/MSVC are not because
// of clang-cl...
#if defined(_MSC_VER) && defined(_MSC_FULL_VER)
#  if _MSC_VER == _MSC_FULL_VER / 10000
#    define GPCL_MSVC                                                          \
      GPCL_COMPILER(_MSC_VER / 100, _MSC_VER % 100, _MSC_FULL_VER % 10000)
#  else // MSVC
#    define GPCL_MSVC                                                          \
      GPCL_COMPILER(_MSC_VER / 100, (_MSC_FULL_VER / 100000) % 100,            \
                    _MSC_FULL_VER % 100000)
#  endif // MSVC
#endif   // MSVC
#if defined(__clang__) && defined(__clang_minor__)
#  define GPCL_CLANG                                                           \
    GPCL_COMPILER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) &&                          \
    defined(__GNUC_PATCHLEVEL__) && !defined(__INTEL_COMPILER)
#  define GPCL_GCC GPCL_COMPILER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#endif // GCC

#ifndef GPCL_MSVC
#  define GPCL_MSVC 0
#endif // GPCL_MSVC
#ifndef GPCL_CLANG
#  define GPCL_CLANG 0
#endif // GPCL_CLANG
#ifndef GPCL_GCC
#  define GPCL_GCC 0
#endif // GPCL_GCC

// =================================================================================================
//  COMPILER WARNINGS HELPERS
// =================================================================================================

#if GPCL_CLANG
#  define GPCL_PRAGMA_TO_STR(x) _Pragma(#  x)
#  define GPCL_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
#  define GPCL_CLANG_SUPPRESS_WARNING(w)                                       \
    GPCL_PRAGMA_TO_STR(clang diagnostic ignored w)
#  define GPCL_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
#  define GPCL_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)                             \
    GPCL_CLANG_SUPPRESS_WARNING_PUSH GPCL_CLANG_SUPPRESS_WARNING(w)
#else // GPCL_CLANG
#  define GPCL_CLANG_SUPPRESS_WARNING_PUSH
#  define GPCL_CLANG_SUPPRESS_WARNING(w)
#  define GPCL_CLANG_SUPPRESS_WARNING_POP
#  define GPCL_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // GPCL_CLANG

#if GPCL_GCC
#  define GPCL_PRAGMA_TO_STR(x) _Pragma(#  x)
#  define GPCL_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
#  define GPCL_GCC_SUPPRESS_WARNING(w)                                         \
    GPCL_PRAGMA_TO_STR(GCC diagnostic ignored w)
#  define GPCL_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
#  define GPCL_GCC_SUPPRESS_WARNING_WITH_PUSH(w)                               \
    GPCL_GCC_SUPPRESS_WARNING_PUSH GPCL_GCC_SUPPRESS_WARNING(w)
#else // GPCL_GCC
#  define GPCL_GCC_SUPPRESS_WARNING_PUSH
#  define GPCL_GCC_SUPPRESS_WARNING(w)
#  define GPCL_GCC_SUPPRESS_WARNING_POP
#  define GPCL_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // GPCL_GCC

#if GPCL_MSVC
#  define GPCL_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
#  define GPCL_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
#  define GPCL_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
#  define GPCL_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)                              \
    GPCL_MSVC_SUPPRESS_WARNING_PUSH GPCL_MSVC_SUPPRESS_WARNING(w)
#else // GPCL_MSVC
#  define GPCL_MSVC_SUPPRESS_WARNING_PUSH
#  define GPCL_MSVC_SUPPRESS_WARNING(w)
#  define GPCL_MSVC_SUPPRESS_WARNING_POP
#  define GPCL_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // GPCL_MSVC

// =================================================================================================
//  FEATURE DETECTION
// =================================================================================================

// general compiler feature support table:
// https://en.cppreference.com/w/cpp/compiler_support MSVC C++11 feature support
// table: https://msdn.microsoft.com/en-us/library/hh567368.aspx GCC C++11
// feature support table: https://gcc.gnu.org/projects/cxx-status.html MSVC
// version table:
// https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B#Internal_version_numbering
// MSVC++ 14.2 (16) _MSC_VER == 1920 (Visual Studio 2019)
// MSVC++ 14.1 (15) _MSC_VER == 1910 (Visual Studio 2017)
// MSVC++ 14.0      _MSC_VER == 1900 (Visual Studio 2015)
// MSVC++ 12.0      _MSC_VER == 1800 (Visual Studio 2013)
// MSVC++ 11.0      _MSC_VER == 1700 (Visual Studio 2012)
// MSVC++ 10.0      _MSC_VER == 1600 (Visual Studio 2010)
// MSVC++ 9.0       _MSC_VER == 1500 (Visual Studio 2008)
// MSVC++ 8.0       _MSC_VER == 1400 (Visual Studio 2005)

#if GPCL_MSVC && !defined(GPCL_CONFIG_WINDOWS_SEH)
#  define GPCL_CONFIG_WINDOWS_SEH
#endif // MSVC
#if defined(GPCL_CONFIG_NO_WINDOWS_SEH) && defined(GPCL_CONFIG_WINDOWS_SEH)
#  undef GPCL_CONFIG_WINDOWS_SEH
#endif // GPCL_CONFIG_NO_WINDOWS_SEH

#if !defined(_WIN32) && !defined(__QNX__) &&                                   \
    !defined(GPCL_CONFIG_POSIX_SIGNALS) && !defined(__EMSCRIPTEN__)
#  define GPCL_CONFIG_POSIX_SIGNALS
#endif // _WIN32
#if defined(GPCL_CONFIG_NO_POSIX_SIGNALS) && defined(GPCL_CONFIG_POSIX_SIGNALS)
#  undef GPCL_CONFIG_POSIX_SIGNALS
#endif // GPCL_CONFIG_NO_POSIX_SIGNALS

#ifndef GPCL_CONFIG_NO_EXCEPTIONS
#  if !defined(__cpp_exceptions) && !defined(__EXCEPTIONS) &&                  \
      !defined(_CPPUNWIND)
#    define GPCL_CONFIG_NO_EXCEPTIONS
#  endif // no exceptions
#endif   // GPCL_CONFIG_NO_EXCEPTIONS

#ifndef GPCL_CONFIG_NO_RTTI
#  if !defined(__cpp_rtti)
#    define GPCL_CONFIG_NO_RTTI
#  endif
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#  if GPCL_MSVC
#    define GPCL_SYMBOL_EXPORT __declspec(dllexport)
#    define GPCL_SYMBOL_IMPORT __declspec(dllimport)
#  else // MSVC
#    define GPCL_SYMBOL_EXPORT __attribute__((dllexport))
#    define GPCL_SYMBOL_IMPORT __attribute__((dllimport))
#  endif // MSVC
#else    // _WIN32
#  define GPCL_SYMBOL_EXPORT __attribute__((visibility("default")))
#  define GPCL_SYMBOL_IMPORT
#endif // _WIN32

#ifdef GPCL_DYN_LINK
#  ifdef GPCL_SOURCE
#    define GPCL_DECL GPCL_SYMBOL_EXPORT
#  else // GPCL_SOURCE
#    define GPCL_DECL GPCL_SYMBOL_IMPORT
#  endif // GPCL_SOURCE
#else    // GPCL_SOURCE
#  define GPCL_DECL
#endif // GPCL_DYN_LINK

#define GPCL_EMPTY

#if GPCL_MSVC
#  define GPCL_NOINLINE __declspec(noinline)
#  define GPCL_UNUSED
#  define GPCL_ALIGNMENT(x)
#elif GPCL_CLANG && GPCL_CLANG < GPCL_COMPILER(3, 5, 0)
#  define GPCL_NOINLINE
#  define GPCL_UNUSED
#  define GPCL_ALIGNMENT(x)
#else
#  define GPCL_NOINLINE __attribute__((noinline))
#  define GPCL_UNUSED __attribute__((unused))
#  define GPCL_ALIGNMENT(x) __attribute__((aligned(x)))
#endif

#ifndef GPCL_NORETURN
#  define GPCL_NORETURN [[noreturn]]
#endif // GPCL_NORETURN

#ifndef GPCL_NOEXCEPT
#  define GPCL_NOEXCEPT noexcept
#endif // GPCL_NOEXCEPT

#ifndef GPCL_NODISCARD
#  define GPCL_NODISCARD [[nodiscard]]
#endif // GPCL_NODISCARD

// =================================================================================================
// FEATURE DETECTION END
// =================================================================================================

#ifndef __STDC_WANT_LIB_EXT1__
#  define __STDC_WANT_LIB_EXT1__ 1
#endif

#ifndef __STDC_WANT_SECURE_LIB__
#  define __STDC_WANT_SECURE_LIB__ 1
#endif

#ifdef _WIN32
#  define GPCL_WINDOWS
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define GPCL_UNIX
#endif

#ifdef __GNUC__
#  ifndef _GNU_SOURCE
#    define _GNU_SOUNCE
#  endif
#endif

#ifdef __linux__
#  define GPCL_LINUX
#endif

#if defined(GPCL_UNIX) && !defined(_GNU_SOUNCE)
#  ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#  endif
#endif

#if defined(GPCL_UNIX)
#  include <unistd.h>
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

#ifndef GPCL_DETAIL_MUTEX_FOR_ADDRESS_USE_SHARED_PTR
#  define GPCL_DETAIL_MUTEX_FOR_ADDRESS_USE_SHARED_PTR 1
#endif

// #undef GPCL_DISABLE_AUTO_LINKING

#if GPCL_MSVC && !defined GPCL_SOURCE && !defined GPCL_DISABLE_AUTO_LINKING
#  pragma comment(lib, "gpcl")
#endif

#ifndef GPCL_BACKTRACE_HEADER
#  define GPCL_BACKTRACE_HEADER <execinfo.h>
#endif

#define GPCL_TO_STR_IMPL(x) #x
#define GPCL_TO_STR(x) GPCL_TO_STR_IMPL(x)

#if GPCL_GCC || GPCL_CLANG
#  define GPCL_THREAD_KEYWORD __thread
#elif GPCL_MSVC
#  define GPCL_THREAD_KEYWORD __declspec(thread)
#else
#  define GPCL_THREAD_KEYWORD thread_local
#endif

#endif // GPCL_DETAIL_CONFIG_HPP
