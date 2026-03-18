//
// function_ref.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_FUNCTION_REF_HPP
#define GPCL_FUNCTION_REF_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {
template <typename Signature>
class function_ref;

template <typename Result, typename... Args>
class function_ref<Result(Args...)>
{
  using invoke_func_t = Result (*)(Args...);

  template <typename T>
  Result invoke(void *p, Args... args)
  {
    return (*reinterpret_cast<T *>(p))(static_cast<Args>(args)...);
  }

  invoke_func_t func_;
  void *p_;

public:
  template <typename T>
  constexpr function_ref(const T &x) : func_(&invoke<T>),
                                       p_(&x)
  {
  }

  Result operator()(Args... args) const
  {
    return func_(static_cast<Args>(args)...);
  }

  const void *target() const noexcept { return p_; }

  void *target() noexcept { return p_; }
};

} // namespace gpcl

#endif // GPCL_FUNCTION_REF_HPP
