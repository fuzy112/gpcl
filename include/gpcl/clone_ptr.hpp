//
// clone_ptr.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CLONE_PTR_HPP
#define GPCL_CLONE_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/propagate_const.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/unique_ptr.hpp>

namespace gpcl {

template <typename T>
struct default_clone
{
  T *operator()(const T *value) const
  {
    if (value)
      return ::new T(*value);
    return nullptr;
  }
};

/// clone_ptr is a smart pointer that automatically clones the managed object.
///
/// @pre
/// T shall be *Cloneable*.
///
/// Deleter Deleter must be *FunctionObject* or lvalue reference to a
/// *FunctionObject* or lvalue reference to function, callable with an argument
/// of type `unique_ptr<T, Deleter>::pointer`.
template <typename T, typename Cloner = default_clone<T>,
          typename Deleter = default_delete<T>>
class clone_ptr
{
public:
  using cloner = Cloner;
  using deleter = Deleter;

  using unique_pointer = unique_ptr<T, deleter>;

  using element_type = T;

  explicit clone_ptr(unique_pointer object, cloner clone = cloner()) noexcept
      : pair_(std::move(object), clone)
  {
  }

  explicit clone_ptr(T *object, cloner c = cloner(), deleter d = deleter()) noexcept
      : pair_(unique_pointer(object, d), c)
  {
  }

  clone_ptr(const clone_ptr &other)
      : clone_ptr(other.get_cloner()(other.operator->()), other.get_cloner(),
                  other.get_deleter())
  {
  }

  clone_ptr(clone_ptr &&other) noexcept
      : clone_ptr(std::move(other.pair_).first(),
                  std::move(other.pair_).second())
  {
  }

  clone_ptr &operator=(const clone_ptr &other)
  {
    clone_ptr(other).swap(*this);
    return *this;
  }

  clone_ptr &operator=(clone_ptr &&other) noexcept
  {
    swap(other);
    return *this;
  }

  void swap(clone_ptr &other) noexcept
  {
    using std::swap;

    swap(pair_.first(), other.pair_.first());
    swap(pair_.second(), other.pair_.second());
  }

  deleter get_deleter() const noexcept { return pair_.first().get_deleter(); }

  cloner get_cloner() const noexcept { return pair_.second(); }

  T *operator->() noexcept { return pair_.first().get(); }

  const T *operator->() const noexcept { return pair_.first().get(); }

  T &operator*() noexcept { return *operator->(); }

  const T &operator*() const noexcept { return *operator->(); }

private:
  detail::compressed_pair<unique_pointer, Cloner> pair_;
};

/// Swap the pointers.
/// @relates clone_ptr
template <typename T>
inline void swap(clone_ptr<T> &x, clone_ptr<T> &y) noexcept
{
  return x.swap(y);
}

} // namespace gpcl

#endif // GPCL_CLONE_PTR_HPP
