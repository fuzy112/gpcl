//
// posix_condition_variable.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_CONDITION_VARIABLE_HPP
#define GPCL_DETAIL_POSIX_CONDITION_VARIABLE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_clock.hpp>
#include <gpcl/detail/posix_mutex.hpp>
#include <gpcl/time.hpp>
#include <gpcl/unique_lock.hpp>

#include <pthread.h>

namespace gpcl {
namespace detail {
class posix_condition_variable
{
public:
  using native_handle_type = pthread_cond_t *;

  GPCL_DECL posix_condition_variable();

  GPCL_DECL ~posix_condition_variable();

  GPCL_DECL void wait(gpcl::unique_lock<posix_normal_mutex> &lock);

  GPCL_DECL bool wait_until(gpcl::unique_lock<posix_normal_mutex> &lock,
                            const system_time &timeout_time);

  GPCL_DECL bool wait_for(gpcl::unique_lock<posix_normal_mutex> &lock,
                          const duration &rel_time);

  GPCL_DECL void notify_one();

  GPCL_DECL void notify_all();

  native_handle_type native_handle() { return &cond_; }

private:
  pthread_cond_t cond_;
};

} // namespace detail
} // namespace gpcl

#endif
