//
// thread_annotations.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_THREAD_ANNOTATIONS_HPP
#define GPCL_DETAIL_THREAD_ANNOTATIONS_HPP

#include <gpcl/detail/config.hpp>

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#  define GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#  define GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(x) // no-op
#endif

#endif // GPCL_DETAIL_THREAD_ANNOTATIONS_HPP
