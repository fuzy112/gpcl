//
// semaphore.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SEMAPHORE_HPP
#define GPCL_SEMAPHORE_HPP

#include <gpcl/detail/config.hpp>

#if defined(GPCL_POSIX)
#  include <gpcl/detail/posix_semaphore.hpp>
#elif defined(GPCL_WINDOWS)
#  include <gpcl/detail/win_semaphore.hpp>
#endif

#include <gpcl/assert.hpp>
#include <gpcl/error.hpp>
#include <gpcl/noncopyable.hpp>

namespace gpcl {

#if defined(GPCL_POSIX)
constexpr std::ptrdiff_t semaphore_default_least_max_value =
    gpcl::detail::posix_semaphore::max();
#elif defined(GPCL_WINDOWS)
constexpr std::ptrdiff_t semaphore_default_least_max_value =
    (gpcl::detail::win_semaphore::max)();
#else
constexpr std::ptrdiff_t semaphore_default_least_max_value = 0;
#endif

template <std::ptrdiff_t LeastMaxValue = semaphore_default_least_max_value>
class counting_semaphore;

#if defined(GPCL_WINDOWS) || defined(GPCL_POSIX)
template <std::ptrdiff_t LeastMaxValue>
class counting_semaphore
{
public:
#  if defined(GPCL_POSIX)
  using impl_type = gpcl::detail::posix_semaphore;
#  elif defined(GPCL_WINDOWS)
  using impl_type = gpcl::detail::win_semaphore;
#  endif
  using value_type = impl_type::value_type;
  using native_handle_type = impl_type::native_handle_type;

  static_assert(LeastMaxValue >= 0);
  static_assert(LeastMaxValue <= impl_type::max());

private:
  impl_type impl_;

public:
  explicit counting_semaphore(std::ptrdiff_t desired) : impl_(desired) {}

  counting_semaphore(const counting_semaphore &) = delete;

  ~counting_semaphore() = delete;

  counting_semaphore &operator=(const counting_semaphore &) = delete;

  void release(std::ptrdiff_t update = 1)
  {
    GPCL_ASSERT(update >= 0);
    GPCL_ASSERT(update <= (max)());

    while (update--)
      impl_.post();
  }

  void acquire() { impl_.wait(); }

  bool try_acquire() noexcept
  {
    GPCL_TRY { return impl_.try_wait(); }
    GPCL_CATCH(...) { return false; }
    GPCL_CATCH_END
    return false;
  }

  static constexpr std::ptrdiff_t max() { return impl_type::max(); }
};

using binary_semaphore = counting_semaphore<1>;

#endif

} // namespace gpcl

#endif
