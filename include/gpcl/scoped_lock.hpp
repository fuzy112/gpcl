//
// scoped_lock.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SCOPED_LOCK_HPP
#define GPCL_SCOPED_LOCK_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/lock.hpp>
#include <gpcl/noncopyable.hpp>

#include <functional>
#include <tuple>

namespace gpcl {
template <typename... MutexTypes>
class scoped_lock : noncopyable
{
  static_assert((is_lockable<MutexTypes>::value && ...) ||
                (sizeof...(MutexTypes) == 1 &&
                 (is_basic_lockable<MutexTypes>::value && ...)));

  std::tuple<MutexTypes &...> mutexes_;

public:
  explicit scoped_lock(MutexTypes &...mutexes) : mutexes_(mutexes...)
  {
    gpcl::lock(mutexes...);
  }

  ~scoped_lock()
  {
    std::apply([](auto &...mutexes) { (mutexes.unlock(), ...); }, mutexes_);
  }
};
} // namespace gpcl

#endif // GPCL_SCOPED_LOCK_HPP
