//
// posix_once_flag.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_ONCE_FLAG_HPP
#define GPCL_DETAIL_IMPL_POSIX_ONCE_FLAG_HPP

#include <gpcl/detail/posix_once_flag.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/unique_lock.hpp>

#if defined(GPCL_LINUX)
#  include <gpcl/detail/futex.hpp>
#endif

#include <atomic>
#include <climits>
#include <exception>

#ifdef __GLIBC__
#  include <gnu/libc-version.h>
#endif

# include <csetjmp>

#include <pthread.h>

namespace gpcl {

template <typename Callable, typename... Args>
void call_once(detail::posix_once_flag &flag, Callable &&callable,
               Args &&...args)
{
  using namespace gpcl::detail;

  std::exception_ptr exc;
  std::jmp_buf jb;

  gpcl::function<void()> func = [&] {
    GPCL_TRY { callable(std::forward<Args>(args)...); }
    GPCL_CATCH(...)
    {
#ifndef __FreeBSD__
      exc = std::current_exception();
      std::longjmp(jb, 1);
#else
      goto cleanup;
      pthread_cleanup_push(NULL, NULL);
cleanup:
      pthread_cleanup_pop(1);
      throw;
#endif
    }
    GPCL_CATCH_END
  };
  ::gpcl::detail::posix_once_functor = &func;
#ifndef __FreeBSD__
  if (setjmp(jb) != 0)
  {

#if defined(__GLIBC__) && __GLIBC__ == 2 && __GLIBC_MINOR__ >= 20
    const int initialization_not_started = PTHREAD_ONCE_INIT;

#  if defined(GPCL_DEBUG)
    const int val = std::atomic_load_explicit(
        reinterpret_cast<std::atomic_int *>(&flag.data_),
        std::memory_order::memory_order_acquire);
    GPCL_ASSERT(val != initialization_not_started);
#  endif

    // Do a store_release indicating that initialization is not yet started.
    std::atomic_store_explicit(reinterpret_cast<std::atomic_int *>(&flag.data_),
                               initialization_not_started,
                               std::memory_order_release);

    // Wake up any waiters, if any.
    int s =
        detail::futex(&flag.data_, FUTEX_WAKE, INT_MAX, nullptr, nullptr, 0);
    if (s == -1)
    {
      GPCL_UNREACHABLE("futex-FUTEX_WAKE");
    }
#elif defined(__CYGWIN__)
    int s = pthread_mutex_unlock(&flag.data_.mutex);
    if (s != 0)
    {
      GPCL_UNREACHABLE("pthread_mutex_unlock");
    }
#else
    { pthread_cleanup_pop(1); }
#endif
    std::rethrow_exception(exc);
  }
#endif

  int err = pthread_once(&flag.data_, []() {
    auto &functor = *::gpcl::detail::posix_once_functor;
    ::gpcl::detail::posix_once_functor = nullptr;
    functor();
  });

  if (err != 0)
    throw_system_error(err, __func__);
}

} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_ONCE_FLAG_HPP
