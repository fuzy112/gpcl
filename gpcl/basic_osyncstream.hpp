#ifndef GPCL_BASIC_OSYNCSTREAM_HPP
#define GPCL_BASIC_OSYNCSTREAM_HPP

#include <gpcl/basic_syncbuf.hpp>

#include <ostream>

namespace gpcl {

namespace detail {

template <typename CharType, typename Traits = std::char_traits<CharType>,
          typename Allocator = std::allocator<CharType>>
class osyncstream_base
{
protected:
  template <typename... Args>
  explicit osyncstream_base(Args &&...args) : buf_(std::forward<Args>(args)...)
  {
  }

  osyncstream_base(osyncstream_base &&) noexcept = default;
  osyncstream_base &operator=(osyncstream_base &&) noexcept = default;

  mutable basic_syncbuf<CharType, Traits, Allocator> buf_;
};

} // namespace detail

template <typename CharType, typename Traits = std::char_traits<CharType>,
          typename Allocator = std::allocator<CharType>>
class basic_osyncstream
    : private detail::osyncstream_base<CharType, Traits, Allocator>,
      public std::basic_ostream<CharType, Traits>
{
public:
  using syncbuf_type = basic_syncbuf<CharType, Traits, Allocator>;
  using streambuf_type = std::basic_streambuf<CharType, Traits>;
  using allocator_type = Allocator;

  basic_osyncstream(streambuf_type *buf, const Allocator &alloc)
      : detail::osyncstream_base<CharType, Traits, Allocator>(buf, alloc),
        std::basic_ostream<CharType, Traits>(rdbuf())
  {
  }

  explicit basic_osyncstream(streambuf_type *buf)
      : detail::osyncstream_base<CharType, Traits, Allocator>(buf),
        std::basic_ostream<CharType, Traits>(rdbuf())
  {
  }

  basic_osyncstream(std::basic_ostream<CharType, Traits> &os,
                    const Allocator &alloc)
      : basic_osyncstream(os.rdbuf(), alloc)
  {
  }

  explicit basic_osyncstream(std::basic_ostream<CharType, Traits> &os)
      : basic_osyncstream(os.rdbuf())
  {
  }

  basic_osyncstream(basic_osyncstream &&other) noexcept
      : detail::osyncstream_base<CharType, Traits, Allocator>(std::move(other)),
        std::basic_ostream<CharType, Traits>(std::move(other))
  {
    this->set_rdbuf(rdbuf());
  }

  basic_osyncstream &operator=(basic_osyncstream &&) noexcept = default;

  syncbuf_type *rdbuf() const noexcept { return &this->buf_; }

  streambuf_type *get_wrapped() const noexcept
  {
    return rdbuf()->get_wrapped();
  }

  void emit()
  {
    if (!rdbuf()->emit())
      this->setstate(std::ios_base::failbit);
  }
};

using osyncstream = basic_osyncstream<char>;
using wosyncstream = basic_osyncstream<wchar_t>;

} // namespace gpcl

#endif // GPCL_BASIC_OSYNCSTREAM_HPP
