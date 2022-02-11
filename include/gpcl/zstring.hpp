//
// zstring.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ZSTRING_HPP
#define GPCL_ZSTRING_HPP

#include <gpcl/detail/config.hpp>

#include <cstddef>
#include <cstdint>

namespace gpcl {

//
// czstring and wzstring
//
// These are "tag" typedefs for C-style strings (i.e. null-terminated character
// arrays) that allow static analysis to help find bugs.
//
// There are no additional features/semantics that we can find a way to add
// inside the type system for these types that will not either incur significant
// runtime costs or (sometimes needlessly) break existing programs when
// introduced.
//

// clang-format off
#if defined _MSC_VER
# pragma warning(push)
# pragma warning(disable : 4245)
#endif
inline constexpr const std::size_t dynamic_extent = -1;
#if defined _MSC_VER
# pragma warning(pop)
#endif
// clang-format on

template <typename CharT, std::size_t Extent = dynamic_extent>
using basic_zstring = CharT *;

template <std::size_t Extent = dynamic_extent>
using czstring = basic_zstring<const char, Extent>;

template <std::size_t Extent = dynamic_extent>
using cwzstring = basic_zstring<const wchar_t, Extent>;

template <std::size_t Extent = dynamic_extent>
using cu16zstring = basic_zstring<const char16_t, Extent>;

template <std::size_t Extent = dynamic_extent>
using cu32zstring = basic_zstring<const char32_t, Extent>;

template <std::size_t Extent = dynamic_extent>
using zstring = basic_zstring<char, Extent>;

template <std::size_t Extent = dynamic_extent>
using wzstring = basic_zstring<wchar_t, Extent>;

template <std::size_t Extent = dynamic_extent>
using u16zstring = basic_zstring<char16_t, Extent>;

template <std::size_t Extent = dynamic_extent>
using u32zstring = basic_zstring<char32_t, Extent>;

} // namespace gpcl

#endif // GPCL_ZSTRING_HPP
