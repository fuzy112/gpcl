//
// out_ptr.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2023-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_OUT_PTR_HPP
#define GPCL_DETAIL_OUT_PTR_HPP

#include <gpcl/apply.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/pointer_traits.hpp>
#include <gpcl/variant.hpp>

#include <tuple>

namespace gpcl {
namespace detail {

template <typename Smart, typename Pointer, typename = void>
struct out_ptr_first_arg;

template <typename Smart, typename Pointer>
struct out_ptr_first_arg<Smart, Pointer, void_t<typename Smart::pointer>>
{
  typedef typename Smart::pointer type;
};

template <typename Smart, typename Pointer, typename = void>
struct out_ptr_first_arg_1;

template <typename Smart, typename Pointer>
struct out_ptr_first_arg_1<Smart, Pointer,
                           void_t<typename Smart::element_type *>>
{
  typedef typename Smart::element_type *type;
};

template <typename Smart, typename Pointer, typename = void>
struct out_ptr_first_arg_2;

template <typename Smart, typename Pointer>
struct out_ptr_first_arg_2<
    Smart, Pointer,
    void_t<typename gpcl::pointer_traits<Smart>::element_type *>>
{
  typedef typename gpcl::pointer_traits<Smart>::element_type *type;
};

template <typename Smart, typename Pointer, typename>
struct out_ptr_first_arg_2
{
  typedef Pointer type;
};

template <typename Smart, typename Pointer, typename>
struct out_ptr_first_arg_1 : out_ptr_first_arg_2<Smart, Pointer>
{
};

template <typename Smart, typename Pointer, typename>
struct out_ptr_first_arg : out_ptr_first_arg_1<Smart, Pointer>
{
};

template <typename Smart, typename X, typename Args, typename = void>
struct out_ptr_is_smart_ptr_resettable : std::false_type
{
};

template <typename Smart, typename X, typename... Args>
struct out_ptr_is_smart_ptr_resettable<
    Smart, X, std::tuple<Args...>,
    void_t<decltype(std::declval<Smart &>().reset(
        std::declval<X>(), std::declval<Args>()...))>> : std::true_type
{
};

template <typename Smart, typename X, typename... Args>
void out_ptr_reset_smart_ptr(
    Smart &sp, X x, std::tuple<Args...> args,
    typename std::enable_if<out_ptr_is_smart_ptr_resettable<
        Smart, X, std::tuple<Args...>>::value>::type * =
        0) noexcept(noexcept(sp.reset(std::declval<X>(), std::declval<Args>()...)))
{
  gpcl::apply([&sp, x = std::move(x)](
                  Args... args) { sp.reset(std::move(x), std::move(args)...); },
              std::move(args));
}

template <typename Smart, typename X, typename... Args>
void out_ptr_reset_smart_ptr(
    Smart &sp, X x, std::tuple<Args...> args,
    typename std::enable_if<!out_ptr_is_smart_ptr_resettable<
        Smart, X, std::tuple<Args...>>::value>::type * =
        0) noexcept(noexcept(std::is_nothrow_constructible<Smart, X,
                                                           Args...>::value))
{
  gpcl::apply(
      [&sp, x = std::move(x)](Args... args) {
        sp = Smart(std::move(x), std::move(args)...);
      },
      std::move(args));
}

template <typename Pointer>
Pointer out_ptr_get_pointer(variant<void *, Pointer> const &ptr) noexcept
{
  return gpcl::visit(
      [](auto &&x) {
        return static_cast<Pointer>(std::forward<decltype(x)>(x));
      },
      ptr);
}

template <typename Smart, typename Pointer, typename... Args>
class out_ptr_t
{
  Smart &smart_ptr_;
  std::tuple<Args...> args_;
  mutable variant<void *, Pointer> ptr_;

public:
  explicit out_ptr_t(Smart &sp, Args... args)
      : smart_ptr_(sp),
        args_(std::move(args)...),
        ptr_()
  {
  }

  out_ptr_t(const out_ptr_t &) = delete;

  out_ptr_t &operator=(const out_ptr_t &) = delete;

  ~out_ptr_t()
  {
    Pointer p = detail::out_ptr_get_pointer(ptr_);
    detail::out_ptr_reset_smart_ptr<
        Smart, typename out_ptr_first_arg<Smart, Pointer>::type, Args...>(
        smart_ptr_, std::move(p), std::move(args_));
  }

  operator Pointer *() const noexcept
  {
    return std::addressof(ptr_.template emplace<Pointer>());
  }

  operator void **() const noexcept
  {
    return std::addressof(ptr_.template emplace<void *>());
  }
};

template <typename Smart, typename Pointer>
struct out_ptr_deduce_helper
{
  typedef Pointer pointer;
};

template <typename Smart>
struct out_ptr_deduce_helper<Smart, void>
{
  typedef typename out_ptr_first_arg<Smart, void>::type pointer;
};

template <typename Pointer = void, typename Smart, typename... Args>
out_ptr_t<Smart, typename out_ptr_deduce_helper<Smart, Pointer>::pointer,
          Args &&...>
out_ptr(Smart &sp, Args &&...args)
{
  return out_ptr_t<Smart,
                   typename out_ptr_deduce_helper<Smart, Pointer>::pointer,
                   Args &&...>(sp, std::forward<Args>(args)...);
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_OUT_PTR_HPP
