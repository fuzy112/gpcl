//
// thread_annotations.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_THREAD_ANNOTATIONS_HPP
#define GPCL_THREAD_ANNOTATIONS_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/thread_annotations.hpp>

#define GPCL_CAPABILITY(x)                                                     \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define GPCL_SCOPED_CAPABILITY                                                 \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GPCL_GUARDED_BY(x)                                                     \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define GPCL_PT_GUARDED_BY(x)                                                  \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define GPCL_ACQUIRED_BEFORE(...)                                              \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define GPCL_ACQUIRED_AFTER(...)                                               \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define GPCL_REQUIRES(...)                                                     \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define GPCL_REQUIRES_SHARED(...)                                              \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(                                   \
      requires_shared_capability(__VA_ARGS__))

#define GPCL_ACQUIRE(...)                                                      \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define GPCL_ACQUIRE_SHARED(...)                                               \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(                                   \
      acquire_shared_capability(__VA_ARGS__))

#define GPCL_RELEASE(...)                                                      \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define GPCL_RELEASE_SHARED(...)                                               \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(                                   \
      release_shared_capability(__VA_ARGS__))

#define GPCL_RELEASE_GENERIC(...)                                              \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(                                   \
      release_generic_capability(__VA_ARGS__))

#define GPCL_TRY_ACQUIRE(...)                                                  \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define GPCL_TRY_ACQUIRE_SHARED(...)                                           \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(                                   \
      try_acquire_shared_capability(__VA_ARGS__))

#define GPCL_EXCLUDES(...)                                                     \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define GPCL_ASSERT_CAPABILITY(x)                                              \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define GPCL_ASSERT_SHARED_CAPABILITY(x)                                       \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define GPCL_RETURN_CAPABILITY(x)                                              \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define GPCL_NO_THREAD_SAFETY_ANALYSIS                                         \
  GPCL_DETAIL_THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

#endif // GPCL_THREAD_ANNOTATIONS_HPP
