//
// posix_semaphore.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_SEMAPHORE_HPP
#define GPCL_DETAIL_POSIX_SEMAPHORE_HPP

#include <gpcl/detail/config.hpp>

#include <cstddef>
#include <cstdint>

#include <limits.h>
#include <semaphore.h>

namespace gpcl {
namespace detail {

class posix_semaphore
{
public:
  using value_type = std::ptrdiff_t;

  GPCL_DECL explicit posix_semaphore(unsigned int init_value);

  GPCL_DECL ~posix_semaphore();

  GPCL_DECL unsigned get_value();

  GPCL_DECL void post();

  GPCL_DECL void wait();

  GPCL_DECL bool try_wait();

  static constexpr std::ptrdiff_t max() noexcept { return SEM_VALUE_MAX; }

  using native_handle_type = sem_t *;
  auto native_handle() -> native_handle_type { return &sem_; }

private:
  ::sem_t sem_{};
};

} // namespace detail
} // namespace gpcl

#endif
