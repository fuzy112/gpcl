//
// creation_tag.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CREATION_TAG_HPP
#define GPCL_CREATION_TAG_HPP

#include <gpcl/static_const.hpp>

namespace gpcl {

struct create_only_t
{
};

struct open_only_t
{
};

struct open_or_create_t
{
};

namespace {
constexpr auto &create_only = static_const<create_only_t>;
constexpr auto &open_only = static_const<open_only_t>;
constexpr auto &open_or_create = static_const<open_or_create_t>;
}

} // namespace gpcl

#endif // GPCL_CREATION_TAG_HPP
