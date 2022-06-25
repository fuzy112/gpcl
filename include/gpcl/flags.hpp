//
// flags.hpp
// ~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_FLAGS_HPP
#define GPCL_FLAGS_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl {

template <typename EnumType>
class basic_flags
{
public:
  using enum_type = EnumType;
  using value_type = typename std::underlying_type<enum_type>::type;

private:
  value_type value_;

public:
  constexpr basic_flags(enum_type e) : value_(static_cast<value_type>(e)) {}

  explicit constexpr basic_flags(value_type v) : value_(v) {}

  constexpr value_type value() const { return value_; }

  constexpr void value(value_type v) { value_ = v; }

  friend inline constexpr basic_flags operator&(const basic_flags &lhs,
                                                const basic_flags &rhs)
  {
    return basic_flags(lhs.value() & rhs.value());
  }

  friend inline constexpr basic_flags operator|(const basic_flags &lhs,
                                                const basic_flags &rhs)
  {
    return basic_flags(lhs.value() | rhs.value());
  }

  friend inline constexpr basic_flags operator^(const basic_flags &lhs,
                                                const basic_flags &rhs)
  {
    return basic_flags(lhs.value() ^ rhs.value());
  }

  explicit constexpr operator bool() const { return bool(value()); }

  friend inline constexpr bool operator==(const basic_flags &lhs,
                                          const basic_flags &rhs)
  {
    return lhs.value() == rhs.value();
  }

  friend inline constexpr bool operator!=(const basic_flags &lhs,
                                          const basic_flags &rhs)
  {
    return lhs.value() == rhs.value();
  }

  constexpr basic_flags &operator&=(const basic_flags &rhs)
  {
    return (*this) = (*this) & rhs;
  }

  constexpr basic_flags &operator|=(const basic_flags &rhs)
  {
    return (*this) = (*this) | rhs;
  }

  constexpr basic_flags &operator^=(const basic_flags &rhs)
  {
    return (*this) = (*this) ^ rhs;
  }

  constexpr bool is_set (enum_type e) const
  {
    return (*this & e) == e;
  }
};

#define GPCL_DEFINE_FLAGS(MyFlags, MyEnum)                                     \
  using MyFlags = basic_flags<MyEnum>;                                         \
                                                                               \
  inline constexpr MyFlags operator&(const MyEnum &lhs, const MyEnum &rhs)     \
  {                                                                            \
    return MyFlags(lhs) & MyFlags(rhs);                                        \
  }                                                                            \
                                                                               \
  inline constexpr MyFlags operator|(const MyEnum &lhs, const MyEnum &rhs)     \
  {                                                                            \
    return MyFlags(lhs) | MyFlags(rhs);                                        \
  }                                                                            \
                                                                               \
  inline constexpr MyFlags operator^(const MyEnum &lhs, const MyEnum &rhs)     \
  {                                                                            \
    return MyFlags(lhs) ^ MyFlags(rhs);                                        \
  }

} // namespace gpcl

#endif // GPCL_FLAGS_HPP
