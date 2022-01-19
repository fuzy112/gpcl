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

#include <gpcl/detail/futex.hpp>
#include <gpcl/detail/posix_once_flag.hpp>
#include <gpcl/unique_lock.hpp>

#include <atomic>
#include <climits>

#ifdef __GLIBC__
#  include <gnu/libc-version.h>
#endif

namespace gpcl {

template <typename Callable, typename... Args>
void call_once(detail::posix_once_flag &flag, Callable &&callable,
               Args &&... args)
{
  using namespace gpcl::detail;

  gpcl::function<void()> func = [&] { callable(std::forward<Args>(args)...); };
  posix_once_functor = &func;
  int err = [&] {
    GPCL_TRY
    {
      return pthread_once(&flag.data_, []() {
        auto &functor = *posix_once_functor;
        posix_once_functor = nullptr;
        functor();
      });
    }
    GPCL_CATCH(...)
    {
      // The following code is a workaround to pthread_once so that we can
      // support C++ exceptions.

#if defined(__GLIBC__) && __GLIBC__ == 2 && __GLIBC_MINOR__ >= 20
      const int initialization_not_started = PTHREAD_ONCE_INIT;

#  if defined(GPCL_DEBUG)
      const int val = std::atomic_load_explicit(
          reinterpret_cast<std::atomic_int *>(&flag.data_),
          std::memory_order::memory_order_acquire);
      GPCL_ASSERT(val != initialization_not_started);
#  endif

      // Do a store_release indicating that initialization is not yet started.
      std::atomic_store_explicit(
          reinterpret_cast<std::atomic_int *>(&flag.data_),
          initialization_not_started, std::memory_order_release);

      // Wake up any waiters, if any.
      int s =
          detail::futex(&flag.data_, FUTEX_WAKE, INT_MAX, nullptr, nullptr, 0);
      if (s == -1)
      {
        GPCL_UNREACHABLE("futex-FUTEX_WAKE");
      }
#else

#  warning "Unsupported version of glibc"
      std::terminate();
#endif

      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }();

  if (err != 0)
    throw_system_error(err, __func__);
}

} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_POSIX_ONCE_FLAG_HPP
