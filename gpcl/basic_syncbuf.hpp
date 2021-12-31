#ifndef GPCL_BASIC_SYNCBUF_HPP
#define GPCL_BASIC_SYNCBUF_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/unique_lock.hpp>

#include <streambuf>
#include <unordered_map>

namespace gpcl {
namespace detail {

template <typename Streambuf>
inline static std::unordered_map<Streambuf *, mutex> streambuf_mutex_map{};

}

template <typename CharType, typename Traits = std::char_traits<CharType>,
          typename Allocator = std::allocator<CharType>>
class basic_syncbuf : public std::basic_streambuf<CharType, Traits>
{
public:
  using allocator_type = Allocator;
  using streambuf_type = std::basic_streambuf<CharType, Traits>;

private:
  enum flags
  {
    none = 0,

    emit_on_sync = 1 << 0,
    pending_flush = 1 << 1,
  };

  std::basic_streambuf<CharType, Traits> *wrapped_;

  unsigned char flag_;

  std::basic_string<CharType, Traits, Allocator> buffer_;

  mutex *mutex_;

public:
  basic_syncbuf() : basic_syncbuf(nullptr) {}

  explicit basic_syncbuf(streambuf_type *buffer,
                         const Allocator &alloc = Allocator())
      : wrapped_(buffer),
        flag_(none),
        buffer_(alloc),
        mutex_(&detail::streambuf_mutex_map<streambuf_type>[buffer])
  {
    this->setp(&buffer_[0], &buffer_[0] + buffer_.size());
  }

  basic_syncbuf(basic_syncbuf &&other)
      : streambuf_type(other),
        wrapped_(other.wrapped_),
        flag_(other.flag_),
        buffer_(std::move(other).buffer_),
        mutex_(other.mutex_)
  {
    other.wrapped_ = false;
    other.flag_ = none;
    other.mutex_ = nullptr;
  }

  basic_syncbuf &operator=(basic_syncbuf &&other)
  {
    emit();

    streambuf_type::operator=(std::move(other));
    wrapped_ = other.wrapped_;
    flag_ = other.flag_;
    buffer_ = std::move(other).buffer_;
    mutex_ = other.mutex_;

    other.wrapped_ = nullptr;
    other.flag_ = none;
    other.mutex_ = nullptr;

    return *this;
  }

  void swap(basic_syncbuf &other) noexcept
  {
    streambuf_type::swap(other);
    gpcl::swap(wrapped_, other.wrapped_);
    gpcl::swap(flag_, other.flag_);
    gpcl::swap(buffer_, other.buffer_);
    gpcl::swap(mutex_, other.mutex_);
  }

  ~basic_syncbuf()
  {
    GPCL_TRY { emit(); }
    GPCL_CATCH(...) {}
    GPCL_CATCH_END
  }

  bool emit()
  {
    if (!wrapped_)
      return false;

    std::streamsize n;
    {
      gpcl::unique_lock<mutex> lock(*mutex_);
      n = wrapped_->sputn(buffer_.data(), buffer_.size());
    }
    buffer_.erase(buffer_.begin(), buffer_.begin() + n);
    if (flag_ & pending_flush)
    {
      flag_ &= ~pending_flush;
      if (this->pubsync() != 0)
        return false;
    }
    return buffer_.empty();
  }

  streambuf_type *get_wrapped() const noexcept { return wrapped_; }

  allocator_type get_allocator() const noexcept
  {
    return buffer_.get_allocator();
  }

  void set_emit_on_sync(bool b) noexcept
  {
    if (b)
      flag_ |= emit_on_sync;
    else
      flag_ &= ~emit_on_sync;
  }

protected:
  int sync() override
  {
    flag_ |= pending_flush;

    if (flag_ & emit_on_sync)
      return emit() ? 0 : -1;

    return 0;
  }

//   typename
//   Traits::int_type
//   overflow()
};

using syncbuf = basic_syncbuf<char>;
using wsyncbuf = basic_syncbuf<wchar_t>;

namespace swap_detail {

template <typename CharType, typename Traits, typename Allocator>
void swap(basic_syncbuf<CharType, Traits, Allocator> &x,
          basic_syncbuf<CharType, Traits, Allocator> &y) noexcept
{
  x.swap(y);
}
} // namespace swap_detail

} // namespace gpcl

#endif // GPCL_BASIC_SYNCBUF_HPP
