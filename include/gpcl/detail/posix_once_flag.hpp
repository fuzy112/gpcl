//
// posix_once_flag.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_ONCE_FLAG_HPP
#define GPCL_DETAIL_POSIX_ONCE_FLAG_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_mutex.hpp>
#include <gpcl/function.hpp>

#include <pthread.h>

namespace gpcl {

namespace detail {
class posix_once_flag;
}

template <typename Callable, typename... Args>
void call_once(detail::posix_once_flag &flag, Callable &&callable,
               Args &&...args);

namespace detail {
class posix_once_flag
{
public:
  explicit constexpr posix_once_flag()
#if defined(__CYGWIN__)
      : data_init_
  {
    19, 0
  }
#else
      : data_
   (
    PTHREAD_ONCE_INIT
   )
#endif
  {
  }

  posix_once_flag(const posix_once_flag &) = delete;
  posix_once_flag &operator=(const posix_once_flag &) = delete;

  template <typename Callable, typename... Args>
  friend void ::gpcl::call_once(posix_once_flag &flag, Callable &&callable,
                                Args &&...args);

  typedef pthread_once_t *native_handle_type;

  native_handle_type native_handle() { return &data_; }

private:
#if defined(__CYGWIN__)
  struct __attribute__((may_alias)) data_s
  {
    long mutex;
    long flag;
  };
  union __attribute__((may_alias))
  {
    data_s data_init_;
    pthread_once_t data_;
  };
#else
  pthread_once_t data_;
#endif
};

inline __thread gpcl::function<void()> *posix_once_functor;

} // namespace detail
} // namespace gpcl

#include <gpcl/detail/impl/posix_once_flag.hpp>

#endif // GPCL_DETAIL_POSIX_ONCE_FLAG_HPP
