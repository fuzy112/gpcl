//
// basic_spanbuf.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BASIC_SPANBUF_HPP
#define GPCL_BASIC_SPANBUF_HPP

#include <gpcl/span.hpp>
#include <gpcl/swap.hpp>

#include <streambuf>

namespace gpcl {

template <typename CharType, typename Traits = std::char_traits<CharType>>
class basic_spanbuf : public std::basic_streambuf<CharType, Traits>
{
  std::ios_base::openmode openmode_;

  using span_type = class span<CharType>;

  span_type span_;

public:
  using pos_type = typename std::basic_streambuf<CharType, Traits>::pos_type;
  using off_type = typename std::basic_streambuf<CharType, Traits>::off_type;

  /// @name Constructors
  /// @{

  basic_spanbuf() : basic_spanbuf(std::ios_base::in | std::ios_base::out) {}

  explicit basic_spanbuf(std::ios_base::openmode which)
      : basic_spanbuf(span_type{}, which)
  {
  }

  explicit basic_spanbuf(span_type buf,
                         std::ios_base::openmode which = std::ios_base::in |
                                                         std::ios_base::out)
      : openmode_(which)
  {
    span(buf);
  }

  basic_spanbuf(const basic_spanbuf &) = delete;

  basic_spanbuf(basic_spanbuf &&other)
      : std::basic_streambuf<CharType, Traits>(other),
        openmode_(other.openmode_),
        span_(other.span_)
  {
    other.openmode_ = std::ios_base::openmode{};
  }

  /// @}

  /// @name Assignment operators
  /// @{

  basic_spanbuf &operator=(const basic_spanbuf &) = delete;

  basic_spanbuf &operator=(basic_spanbuf &&other)
  {
    basic_spanbuf(std::move(other)).swap(this);
    return *this;
  }

  /// @}

  void swap(basic_spanbuf &other)
  {
    std::basic_streambuf<CharType, Traits>::swap(other);
    gpcl::swap(openmode_, other.openmode_);
    gpcl::swap(span_, other.span_);
  }

  void span(span<CharType> buf) noexcept
  {
    span_ = buf;

    CharType *begin = span_.data();
    CharType *end = begin + span_.size();

    if (openmode_ & std::ios_base::in)
    {
      this->setg(begin, begin, end);
    }

    if (openmode_ & std::ios_base::out)
    {
      if (openmode_ & std::ios_base::ate)
      {
        this->setp(end, end);
      }
      else
      {
        this->setp(begin, end);
      }
    }
  }

  span_type span() const noexcept
  {
    if (openmode_ & std::ios_base::out)
    {
      return span_type(this->pbase(), this->pptr());
    }

    return span_;
  }

protected:
  std::basic_streambuf<CharType, Traits> *setbuf(CharType *s,
                                                 std::streamsize n) override
  {
    this->span(span_type(s, n));
    return this;
  }

  pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                   std::ios_base::openmode which = std::ios_base::in |
                                                   std::ios_base::out) override
  {
    if ((openmode_ & which) != which)
      return off_type(-1);

    off_type newoff;
    switch (dir)
    {
    case std::ios_base::beg:
      newoff = off;
      break;

    case std::ios_base::cur:
      if ((which & std::ios_base::in) && (which & std::ios_base::out))
        return off_type(-1);

      if (which & std::ios_base::out)
        newoff = this->pptr() - this->pbase() + off;
      else if (which & std::ios_base::in)
        newoff = this->gptr() - this->eback() + off;
      else
        return off_type(-1);
      break;

    case std::ios_base::end:
      if ((openmode_ & std::ios_base::out) && !(openmode_ & std::ios_base::in))
        newoff = this->pptr() - this->pbase() + off;
      else
        newoff = off + span_.size();
      break;

    default:
      GPCL_UNREACHABLE("invalid seekdir");
    }

    if (this->pptr() == nullptr && this->gptr() == nullptr && newoff != 0)
      return off_type(-1);

    if (newoff < 0 || std::size_t(newoff) > span_.size())
      return off_type(-1);

    CharType *pbuf = span_.data();
    if (which & std::ios_base::in)
      this->setg(pbuf, pbuf + newoff, pbuf + span_.size());

    if (which & std::ios_base::out)
      this->setp(pbuf + newoff, pbuf + span_.size());

    return newoff;
  }

  pos_type
  seekpos(pos_type pos,
          std::ios_base::openmode which = std::ios_base::in |
                                          std::ios_base::out) override
  {
    return seekoff(off_type(pos), std::ios_base::beg, which);
  }
};

using spanbuf = basic_spanbuf<char>;
using wspanbuf = basic_spanbuf<wchar_t>;

namespace swap_detail {
template <typename CharType, typename Traits>
void swap(basic_spanbuf<CharType, Traits> &x,
          basic_spanbuf<CharType, Traits> &y)
{
  x.swap(y);
}
} // namespace swap_detail

} // namespace gpcl

#endif // GPCL_BASIC_SPANBUF_HPP
