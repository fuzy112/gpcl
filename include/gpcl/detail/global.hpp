//
// global.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_GLOBAL_HPP
#define GPCL_DETAIL_GLOBAL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/once_flag.hpp>

namespace gpcl::detail {
template <typename T>
struct global_impl
{
  global_impl() : ptr_() {}

  ~global_impl() { delete static_ptr_; }

  void do_init() { ptr_ = static_ptr_ = new T(); }

  static global_impl instance_;
  static once_flag init_once_;
  static T *static_ptr_;
  T *ptr_;
};

template <typename T>
global_impl<T> global_impl<T>::instance_;

template <typename T>
once_flag global_impl<T>::init_once_;

template <typename T>
T *global_impl<T>::static_ptr_ = nullptr;

template <typename T>
T &global()
{
  call_once(global_impl<T>::init_once_, &global_impl<T>::do_init);

  return *global_impl<T>::instance_.ptr_;
}
} // namespace gpcl::detail

#endif // GPCL_DETAIL_GLOBAL_HPP
