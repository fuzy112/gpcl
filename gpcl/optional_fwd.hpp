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
#include <initializer_list>
#include <type_traits>

namespace gpcl {
template <typename T>
class optional;

enum class nullopt_t
{
  nullopt
};
constexpr nullopt_t nullopt = nullopt_t::nullopt;

// class bad_optional_access
class bad_optional_access;

// hash support
template <typename T>
struct hash;
template <typename T>
struct hash<optional<T>>;

} // namespace gpcl

#endif // GPCL_OPTIONAL_FWD_HPP
