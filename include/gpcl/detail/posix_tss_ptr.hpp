//
// posix_tss_ptr.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_TSS_PTR_HPP
#define GPCL_DETAIL_POSIX_TSS_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_once_flag.hpp>
#include <gpcl/detail/throw_system_error.hpp>

#include <pthread.h>

namespace gpcl::detail {

class posix_tss_key
{
public:
  constexpr posix_tss_key() = default;

  ~posix_tss_key()
  {
#ifdef GPCL_CONFIG_DISALLOW_LEAK
    GPCL_TRY { init_once(); }
    GPCL_CATCH(...) { return; }
    GPCL_CATCH_END
    if (pthread_key_delete(key_) != 0)
      abort();
#endif
  }

  void *get() const noexcept
  {
    init_once();
    return ::pthread_getspecific(key_);
  }

  void set(const void *value)
  {
    init_once();
    int err = pthread_setspecific(key_, value);
    if (err != 0)
      GPCL_THROW_ERRNO(err, "pthread_setspecific");
  }

private:
  void init_once() const
  {
    call_once(once_, [this] {
      int err = pthread_key_create(&key_, nullptr);
      if (err != 0)
        GPCL_THROW_ERRNO(err, "pthread_key_create");
    });
  }
  mutable posix_once_flag once_{};
  mutable pthread_key_t key_{};
};

template <typename T>
class posix_tss_ptr
{
public:
  constexpr posix_tss_ptr() = default;

  ~posix_tss_ptr() = default;

  posix_tss_ptr(const posix_tss_ptr &) = delete;

  posix_tss_ptr &operator=(const posix_tss_ptr &) = delete;

  operator T *() const noexcept
  {
    return static_cast<T *>(key_.get());
  }

  void operator=(T *value)
  {
    key_.set(value);
  }

private:
  static posix_tss_key key_;
};

template <typename T>
posix_tss_key posix_tss_ptr<T>::key_;

} // namespace gpcl::detail

#endif // GPCL_DETAIL_POSIX_TSS_PTR_HPP
