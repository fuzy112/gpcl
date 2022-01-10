//
// function.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_FUNCTION_HPP
#define GPCL_FUNCTION_HPP

#include <gpcl/basic_any.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/swap.hpp>

#include <tuple>

namespace gpcl {

class bad_function_call : public std::exception
{
public:
  bad_function_call() = default;

  bad_function_call(const bad_function_call &other) noexcept = default;

  bad_function_call &
  operator=(const bad_function_call &other) noexcept = default;

  const char *what() const noexcept override { return "bad_function_call"; }
};

template <typename Signature, std::size_t LocalSize = 3 * sizeof(void *)>
class function;

template <typename Signature, std::size_t LocalSize>
void swap(function<Signature, LocalSize> &x,
          function<Signature, LocalSize> &y) noexcept;

/** @tparam LocalSize the maximum size of target that be stored locally.
 *
 *  @todo support member function pointer
 */
template <typename Result, typename... Args, std::size_t LocalSize>
class function<Result(Args...), LocalSize>
{
  using any_t = basic_any<LocalSize>;
  using invoke_t = Result (*)(const void *, Args...);

  any_t data_;
  invoke_t invoke_ = nullptr;

public:
  using result_type = Result;

  /// @name Constructors
  /// @{

  constexpr function() noexcept = default;

  function(const function &other) = default;

  function(function &&other) noexcept
      : data_(std::move(other.data_)),
        invoke_(gpcl::detail::exchange(other.invoke_, nullptr))
  {
  }

  function(std::nullptr_t) noexcept {}

  template <typename F>
  function(F &&f) : data_(std::forward<F>(f))
  {
    invoke_ = [](const void *pf, Args... args) -> Result {
      auto f = static_cast<const std::decay_t<F> *>(pf);
      return (*f)(std::move(args)...);
    };
  }

  /// @}

  /// @name Assignement Operators
  /// @{

  function &operator=(const function &other) = default;

  function &operator=(function &&other) noexcept
  {
    data_ = std::move(other.data_);
    invoke_ = detail::exchange(other.invoke_, nullptr);
    return *this;
  }

  /// @}

  /// @name Modifiers
  void swap(function &other) noexcept
  {
    using gpcl::swap;
    swap(data_, other.data_);
    swap(invoke_, other.invoke_);
  }

  /// @name Target access
  /// @{

  const std::type_info &target_type() const noexcept { return data_.type(); }

  const void *target() const noexcept { return data_.raw_value(); }

  void *target() noexcept { return data_.raw_value(); }

  /// @}

  /// Determines whether the function owns a target.
  explicit operator bool() const noexcept { return data_.has_value(); }

  /// Invokes the target function.
  /// @throws bad_function_call if `bool(*this)` is false.
  result_type operator()(Args... args) const
  {
    if (const void *value = data_.raw_value())
      return invoke_(value, std::move(args)...);
    GPCL_THROW(bad_function_call());
  }
};

/// @name Relational Operators
/// Compares a @c function with @c nullptr.
/// @relates {gpcl::function< Result(Args...), LocalSize >}
/// @{

template <typename R, typename... Args, std::size_t LocalSize>
bool operator==(const function<R(Args...), LocalSize> &f,
                std::nullptr_t) noexcept
{
  return !f;
}

template <typename R, typename... Args, std::size_t LocalSize>
bool operator==(std::nullptr_t,
                const function<R(Args...), LocalSize> &f) noexcept
{
  return !f;
}

template <typename R, typename... Args, std::size_t LocalSize>
bool operator!=(const function<R(Args...), LocalSize> &f,
                std::nullptr_t) noexcept
{
  return f;
}

template <typename R, typename... Args, std::size_t LocalSize>
bool operator!=(std::nullptr_t,
                const function<R(Args...), LocalSize> &f) noexcept
{
  return f;
}

/// @}

/// @relates function
template <typename Signature, std::size_t LocalSize>
void swap(function<Signature, LocalSize> &x,
          function<Signature, LocalSize> &y) noexcept
{
  x.swap(y);
}

} // namespace gpcl

#endif // GPCL_FUNCTION_HPP
