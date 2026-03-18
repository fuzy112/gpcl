//
// json_tag.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_JSON_TAG_HPP
#define GPCL_JSON_TAG_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {
struct json_null_tag
{
};
struct json_integer_tag
{
};
struct json_real_tag
{
};
struct json_string_tag
{
};
struct json_boolean_tag
{
};
struct json_array_tag
{
};
struct json_object_tag
{
};
} // namespace gpcl

#endif // GPCL_JSON_TAG_HPP
