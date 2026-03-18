//
// scope_exit.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SCOPE_EXIT_HPP
#define GPCL_SCOPE_EXIT_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {

template <typename F>
class scope_exit
{
public:
  static_assert(std::is_destructible_v<F> || std::is_lvalue_reference_v<F>);

private:
  F function_;
  bool active_ = true;

public:
  template <typename Fn,
            std::enable_if_t<!std::is_same_v<std::decay_t<Fn>, scope_exit> &&
                                 std::is_constructible_v<F, Fn>,
                             int> = 0>
  explicit scope_exit(Fn &&fn) noexcept(
      std::is_nothrow_constructible_v<F, Fn> ||
      std::is_nothrow_constructible_v<F, Fn &>)
      : function_(static_cast<
                  std::conditional_t<!std::is_lvalue_reference_v<F> &&
                                         std::is_nothrow_constructible_v<F, Fn>,
                                     Fn &&, Fn &>>(fn))
  {
  }

  scope_exit(scope_exit &&other) noexcept(
      std::is_nothrow_move_constructible_v<F> ||
      std::is_nothrow_copy_assignable_v<F>)
      : function_(
            static_cast<std::conditional_t<std::is_nothrow_move_assignable_v<F>,
                                           F &&, F &>>(other.function_)),
        active_(other.active_)
  {
    other.release();
  }

  scope_exit(scope_exit const &) = delete;

  ~scope_exit()
  {
    if (active_)
    {
      active_ = false;
      function_();
    }
  }

  void operator=(const scope_exit &) = delete;

  void release() noexcept { active_ = false; }
};

template <typename F>
scope_exit(F) -> scope_exit<F>;

} // namespace gpcl

#endif // GPCL_SCOPE_EXIT_HPP
