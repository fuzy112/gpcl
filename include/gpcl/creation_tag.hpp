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

inline constexpr create_only_t create_only{};
inline constexpr open_only_t open_only{};
inline constexpr open_or_create_t open_or_create{};

} // namespace gpcl

#endif // GPCL_CREATION_TAG_HPP
