//
// posix_tss_ptr.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_TSS_PTR_HPP
#define GPCL_DETAIL_POSIX_TSS_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/pthread_error.hpp>
#include <gpcl/detail/throw_system_error.hpp>

#include <pthread.h>

namespace gpcl::detail {

template <typename T>
class posix_tss_ptr
{
public:
  posix_tss_ptr()
  {
    GPCL_THROW_IF_PTHREAD_FAILED(::pthread_key_create(&key_, nullptr));
  }

  ~posix_tss_ptr() { (void)::pthread_key_delete(key_); }

  posix_tss_ptr(const posix_tss_ptr &) = delete;

  posix_tss_ptr &operator=(const posix_tss_ptr &) = delete;

  operator T *() const noexcept
  {
    return static_cast<T *>(::pthread_getspecific(key_));
  }

  void operator=(T *value)
  {
    GPCL_THROW_IF_PTHREAD_FAILED(pthread_setspecific(key_, value));
  }

private:
  ::pthread_key_t key_;
};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_POSIX_TSS_PTR_HPP
