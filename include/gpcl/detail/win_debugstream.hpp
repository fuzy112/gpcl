//
// win_debugstream.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_DEBUGSTREAM_HPP
#define GPCL_DETAIL_WIN_DEBUGSTREAM_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/win_debug_streambuf.hpp>

#include <ostream>

namespace gpcl::detail {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_win_debugstream : private basic_win_debug_streambuf<CharT, Traits>,
                              public std::basic_ostream<CharT, Traits>
{
public:
  basic_win_debugstream() : std::basic_ostream<CharT, Traits>(this) {}
};

using win_debugstream = basic_win_debugstream<char>;

inline win_debugstream cwindebug{};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_WIN_DEBUGSTREAM_HPP
