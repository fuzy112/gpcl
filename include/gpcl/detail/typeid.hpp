//
// typeid.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_TYPEID_HPP
#define GPCL_DETAIL_TYPEID_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>

#include <functional>

namespace gpcl::detail {

template <typename T>
struct typeid_impl;

enum class type_info_init
{
  init,
};

struct type_info : noncopyable
{
  template <typename T>
  friend struct typeid_impl;

  virtual ~type_info() = default;

  const char *name() const noexcept { return "unknown"; }

  constexpr bool before(const type_info &other) const noexcept
  {
    return this < &other;
  }

  constexpr bool operator==(const type_info &other) const noexcept
  {
    return this == &other;
  }

  constexpr bool operator!=(const type_info &other) const noexcept
  {
    return this != &other;
  }

  std::size_t hash_code() const noexcept
  {
    return std::hash<const type_info *>()(this);
  }

protected:
  constexpr explicit type_info() noexcept {}
};


template <typename T>
struct typeid_impl
{
  static type_info typeid_;
};

template <typename T>
type_info typeid_impl<T>::typeid_;

} // namespace gpcl::detail

#endif // GPCL_DETAIL_TYPEID_HPP
