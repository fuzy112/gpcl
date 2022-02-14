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

#include <atomic>
#include <climits>
#include <csetjmp>
#include <exception>

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
#if defined(__GLIBC__) || defined(__CYGWIN__)
      exc = std::current_exception();
      std::longjmp(jb, 1);
#else
      goto cleanup;
      pthread_cleanup_push(NULL, NULL);
    cleanup:
      pthread_cleanup_pop(1);
      GPCL_RETHROW;
#endif
    }
    GPCL_CATCH_END
  };
  ::gpcl::detail::posix_once_functor = &func;

#if defined(__GLIBC__) || defined(__CYGWIN__)
  if (setjmp(jb) > 0)
  {

#  if defined(__CYGWIN__)
    int s = pthread_mutex_unlock(&flag.data_.mutex);
    if (s != 0)
    {
      GPCL_UNREACHABLE("pthread_mutex_unlock");
    }
#  endif

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
