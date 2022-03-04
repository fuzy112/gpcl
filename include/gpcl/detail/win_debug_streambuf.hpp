//
// win_debug_streambuf.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_DEBUG_STREAMBUF_HPP
#define GPCL_DETAIL_WIN_DEBUG_STREAMBUF_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>

#include <iostream>
#include <sstream>
#include <streambuf>


namespace gpcl::detail {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_win_debug_streambuf;

template <typename Traits>
class basic_win_debug_streambuf<char, Traits>
    : public std::basic_stringbuf<char, Traits>
{
  static_assert(std::is_same<char, typename Traits::char_type>::value, "");

public:
  basic_win_debug_streambuf() = default;

  ~basic_win_debug_streambuf()
  {
    GPCL_TRY { sync(); }
    GPCL_CATCH(...) {}
    GPCL_CATCH_END
  }

protected:
  int sync() override final
  {
    if (0 != std::basic_streambuf<char, Traits>::sync())
      return -1;

    std::string s;
    for (char c : this->str())
    {
      if (c == '\n')
      {
        s.push_back('\r');
        s.push_back('\n');
      }
      else
      {
        s.push_back(c);
      }
    }

    OutputDebugStringA(s.c_str());

    std::clog << this->str();
    this->str(std::basic_string<char, Traits>());

    return 0;
  }
};

using win_debug_streambuf = basic_win_debug_streambuf<char>;

} // namespace gpcl::detail

#endif
