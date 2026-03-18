//
// output_iterator_streambuf.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ITERATOR_STREAMbuf_HPP
#define GPCL_DETAIL_ITERATOR_STREAMbuf_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/span.hpp>

#include <iterator>
#include <streambuf>

namespace gpcl {
namespace detail {

template <typename OutIt, typename CharType = char,
          typename CharTraits = std::char_traits<CharType>>
class output_iterator_streambuf
    : public std::basic_streambuf<CharType, CharTraits>
{
  using base_type = std::basic_streambuf<CharType, CharTraits>;

public:
  using char_type = CharType;
  using int_type = typename CharTraits::int_type;

  explicit output_iterator_streambuf(OutIt out) : out_(out) {}

  OutIt out()
  {
    // sync() never fails
    output_iterator_streambuf::sync();
    return out_;
  }

protected:
  output_iterator_streambuf *setbuf(char_type *s, std::streamsize n) override
  {
    base_type::setp(s, s + n);
    return this;
  }

  int_type overflow(int_type ch = CharTraits::eof()) override
  {
    out_ = std::copy(base_type::pbase(), base_type::pptr(), out_);
    if (ch != CharTraits::eof())
    {
      *out_++ = CharTraits::to_char_type(ch);
    }
    base_type::setp(base_type::pbase(), base_type::epptr());
    return 0;
  }

  int sync() override
  {
    if (output_iterator_streambuf::overflow() == CharTraits::eof())
      return -1; // should never happens;
    return 0;
  }

private:
  OutIt out_;
};

} // namespace detail
} // namespace gpcl

#endif
