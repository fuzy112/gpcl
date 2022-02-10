//
// scope_fail.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SCOPE_FAIL_HPP
#define GPCL_SCOPE_FAIL_HPP
#include <gpcl/detail/config.hpp>

#include <exception>
#include <type_traits>

namespace gpcl {

template <typename F>
class scope_fail
{
public:
  static_assert(std::is_destructible_v<F> || std::is_lvalue_reference_v<F>, "");

private:
  F function_;
  const int uncaught_exceptions_ = std::uncaught_exceptions();
  bool active_ = true;

public:
  template <typename Fn,
            std::enable_if_t<!std::is_same_v<std::decay_t<Fn>, scope_fail> &&
                                 std::is_constructible_v<F, Fn>,
                             int> = 0>
  explicit scope_fail(Fn &&fn) noexcept(
      std::is_nothrow_constructible_v<F, Fn> ||
      std::is_nothrow_constructible_v<F, Fn &>)
      : function_(static_cast<
                  std::conditional_t<!std::is_lvalue_reference_v<F> &&
                                         std::is_nothrow_constructible_v<F, Fn>,
                                     Fn &&, Fn &>>(fn))
  {
  }

  scope_fail(scope_fail &&other) noexcept(
      std::is_nothrow_move_constructible_v<F> ||
      std::is_nothrow_copy_assignable_v<F>)
      : function_(
            static_cast<std::conditional_t<std::is_nothrow_move_assignable_v<F>,
                                           F &&, F &>>(other.function_)),
        active_(other.active_),
        uncaught_exceptions_(other.uncaught_exceptions_)
  {
    other.release();
  }

  scope_fail(scope_fail const &) = delete;

  ~scope_fail()
  {
    if (active_ && uncaught_exceptions_ < std::uncaught_exceptions())
    {
      active_ = false;
      function_();
    }
  }

  void operator=(const scope_fail &) = delete;

  void release() noexcept { active_ = false; }
};

#if __cplusplus >= 201703
template <typename F>
scope_fail(F) -> scope_fail<F>;
#endif

} // namespace gpcl

#endif // GPCL_SCOPE_FAIL_HPP
