//
// futex_mutex.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_FUTEX_MUTEX_HPP
#define GPCL_DETAIL_FUTEX_MUTEX_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/futex.hpp>
#include <gpcl/inttypes.hpp>

namespace gpcl {
namespace detail {

class futex_mutex
{
public:
  typedef futex_word_type *native_handle_type;

  inline constexpr futex_mutex() noexcept;

  futex_mutex(const futex_mutex &) = delete;
  futex_mutex(futex_mutex &&) = delete;

  futex_mutex &operator=(const futex_mutex &) = delete;
  futex_mutex &operator=(futex_mutex &&) = delete;

  GPCL_DECL void lock() noexcept;

  GPCL_DECL void unlock() noexcept;

  GPCL_DECL bool try_lock() noexcept;

  native_handle_type native_handle() noexcept { return &fut_; }

private:
  futex_word_type fut_;
};

inline constexpr const futex_word_type futex_locked = 0;
inline constexpr const futex_word_type futex_unlocked = 1;

constexpr futex_mutex::futex_mutex() noexcept : fut_(futex_unlocked) {}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_FUTEX_MUTEX_HPP
