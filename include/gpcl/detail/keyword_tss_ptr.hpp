//
// keyword_tss_ptr.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_KEYWORD_TSS_PTR_HPP
#define GPCL_DETAIL_KEYWORD_TSS_PTR_HPP

#include <gpcl/detail/config.hpp>

#if GPCL_GCC || GPCL_CLANG
#  define GPCL_TSS_KEYWORD __thread
#else
#  define GPCL_TSS_KEYWORD thread_local
#endif

namespace gpcl::detail {

template <typename T>
class keyword_tss_ptr
{
public:
  constexpr keyword_tss_ptr() = default;

  ~keyword_tss_ptr() = default;

  keyword_tss_ptr(const keyword_tss_ptr &) = delete;
  keyword_tss_ptr &operator=(const keyword_tss_ptr &) = delete;

  operator T *() const noexcept { return value_; }

  void operator=(T *value) noexcept { value_ = value; }

private:
  static GPCL_TSS_KEYWORD T *value_;
};

template <typename T>
GPCL_TSS_KEYWORD T* keyword_tss_ptr<T>::value_;

} // namespace gpcl::detail

#endif // GPCL_DETAIL_KEYWORD_TSS_PTR_HPP
