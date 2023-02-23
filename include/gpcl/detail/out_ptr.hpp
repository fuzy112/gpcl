//
// out_ptr.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2023 Zhengyi Fu (tsingyat at outlook dot com)
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

#include <tuple>

namespace gpcl {
namespace detail {

template <typename Smart, typename Pointer, typename = void>
struct first_reset_arg;

template <typename Smart, typename Pointer>
struct first_reset_arg<Smart, Pointer, void_t<typename Smart::pointer>>
{
  typedef typename Smart::pointer type;
};

template <typename Smart, typename Pointer, typename = void>
struct first_reset_arg_1;

template <typename Smart, typename Pointer>
struct first_reset_arg_1<Smart, Pointer, void_t<typename Smart::element_type *>>
{
  typedef typename Smart::element_type *type;
};

template <typename Smart, typename Pointer, typename = void>
struct first_reset_arg_2;

template <typename Smart, typename Pointer>
struct first_reset_arg_2<
    Smart, Pointer,
    void_t<typename gpcl::pointer_traits<Smart>::element_type *>>
{
  typedef typename gpcl::pointer_traits<Smart>::element_type *type;
};

template <typename Smart, typename Pointer, typename>
struct first_reset_arg_2
{
  typedef Pointer type;
};

template <typename Smart, typename Pointer, typename>
struct first_reset_arg_1 : first_reset_arg_2<Smart, Pointer>
{
};

template <typename Smart, typename Pointer, typename>
struct first_reset_arg : first_reset_arg_1<Smart, Pointer>
{
};

template <typename Smart, typename X, typename Args, typename = void>
struct is_resettable : std::false_type
{
};

template <typename Smart, typename X, typename... Args>
struct is_resettable<Smart, X, std::tuple<Args...>,
                     void_t<decltype(std::declval<Smart &>().reset(
                         std::declval<X>(), std::declval<Args>()...))>>
    : std::true_type
{
};

template <typename Smart, typename X, typename... Args>
void do_reset(
    Smart &sp, X x, std::tuple<Args...> args,
    typename std::enable_if<
        is_resettable<Smart, X, std::tuple<Args...>>::value>::type * = nullptr)
{
  gpcl::apply([&sp, x = std::move(x)](
                  Args... args) { sp.reset(std::move(x), std::move(args)...); },
              std::move(args));
}

template <typename Smart, typename X, typename... Args>
void do_reset(
    Smart &sp, X x, std::tuple<Args...> args,
    typename std::enable_if<
        !is_resettable<Smart, X, std::tuple<Args...>>::value>::type * = nullptr)
{
  gpcl::apply(
      [&sp, x = std::move(x)](Args... args) {
        sp = Smart(std::move(x), std::move(args)...);
      },
      std::move(args));
}

template <typename Smart, typename Pointer, typename... Args>
class out_ptr_t
{
  Smart &smart_ptr_;
  std::tuple<Args...> args_;
  union
  {
    mutable Pointer ptr_;
    mutable void *void_;
  };
  mutable bool is_void_;

public:
  explicit out_ptr_t(Smart &sp, Args... args)
      : smart_ptr_(sp),
        args_(std::move(args)...),
        ptr_(),
        is_void_()
  {
  }

  out_ptr_t(const out_ptr_t &) = delete;

  out_ptr_t &operator=(const out_ptr_t &) = delete;

  ~out_ptr_t()
  {
    Pointer p = [this] {
      if (is_void_)
      {
        return static_cast<Pointer>(void_);
      }
      else
      {
        Pointer p = std::move(ptr_);
        ptr_.~Pointer();
        return p;
      }
    }();

    do_reset<Smart, typename first_reset_arg<Smart, Pointer>::type, Args...>(
        smart_ptr_, std::move(p), std::move(args_));
  }

  operator Pointer *() const noexcept
  {
    is_void_ = false;
    return &ptr_;
  }

  operator void **() const noexcept
  {
    is_void_ = true;
    return &void_;
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
  typedef typename first_reset_arg<Smart, void>::type pointer;
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
