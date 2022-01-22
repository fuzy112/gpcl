#ifndef GPCL_DETAIL_WIN_DEBUG_STREAMBUF_HPP
#define GPCL_DETAIL_WIN_DEBUG_STREAMBUF_HPP

#include <gpcl/detail/config.hpp>

#include <streambuf>
#include <sstream>
#include <iostream>

#include <debugapi.h>

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
