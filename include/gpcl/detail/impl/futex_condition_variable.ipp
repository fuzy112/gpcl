//
// futex_condition_variable.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_FUTEX_CONDITION_VARIABLE_IPP
#define GPCL_DETAIL_IMPL_FUTEX_CONDITION_VARIABLE_IPP

#include <gpcl/detail/futex_condition_variable.hpp>
#include <limits>

namespace gpcl {
namespace detail {

void futex_condition_variable::wait(futex_mutex &mtx)
{
  struct locker
  {
    futex_mutex &mtx;

    explicit locker(futex_mutex &mtx) : mtx(mtx) { mtx.unlock(); }

    ~locker() { mtx.lock(); }
  };

  futex_word_type one = 1;
  if (__atomic_compare_exchange_n(&fut_, &one, 0, true, __ATOMIC_CONSUME,
                                  __ATOMIC_RELAXED))
  {
    return;
  }
  else
  {
    locker l{mtx};
    futex(&fut_, FUTEX_WAIT, 0, nullptr, nullptr, 0);
    __atomic_store_n(&fut_, 0, __ATOMIC_RELAXED);
  }
}

void futex_condition_variable::notify_all()
{
  __atomic_store_n(&fut_, 1, __ATOMIC_RELAXED);
  futex(&fut_, FUTEX_WAKE, std::numeric_limits<futex_word_type>::max(), nullptr,
        nullptr, 0);
}

void futex_condition_variable::notify_one()
{
  __atomic_store_n(&fut_, 1, __ATOMIC_RELAXED);
  futex(&fut_, FUTEX_WAKE, 1, nullptr, nullptr, 0);
}

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_FUTEX_CONDITION_VARIABLE_IPP
