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
