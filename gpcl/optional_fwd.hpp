//
// optional_fwd.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OPTIONAL_FWD_HPP
#define GPCL_OPTIONAL_FWD_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/in_place.hpp>

#include <type_traits>

namespace gpcl {

namespace detail {
struct optional_construct_helper
{
  explicit optional_construct_helper() = default;
};
} // namespace detail

template <typename T>
class optional;

struct nullopt_t
{
  constexpr nullopt_t(detail::optional_construct_helper) {}
};
constexpr nullopt_t nullopt{detail::optional_construct_helper{}};

template <typename T>
struct is_optional : std::false_type
{
};

template <typename T>
struct is_optional<optional<T>> : std::true_type
{
};

template <typename T>
constexpr bool is_optional_v = is_optional<T>::value;

// class bad_optional_access
class bad_optional_access;

// hash support
template <typename T>
struct hash;
template <typename T>
struct hash<optional<T>>;

} // namespace gpcl

#endif // GPCL_OPTIONAL_FWD_HPP
