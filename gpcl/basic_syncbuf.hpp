#ifndef GPCL_BASIC_SYNCBUF_HPP
#define GPCL_BASIC_SYNCBUF_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/unique_lock.hpp>
#include <gpcl/weak_ptr.hpp>

#include <streambuf>
#include <unordered_map>

namespace gpcl {
namespace detail {

template <typename CharType, typename Traits>
shared_ptr<mutex>
get_mutex_for(std::basic_streambuf<CharType, Traits> const *streambuf)
{
  using streambuf_type = std::basic_streambuf<CharType, Traits>;

  static recursive_mutex map_mutex;
  static std::unordered_map<const streambuf_type *, weak_ptr<mutex>> map;

  unique_lock<recursive_mutex> lock(map_mutex);

  auto &wp = map[streambuf];
  if (auto sp = wp.lock())
    return sp;

  struct mutex_deleter
  {
    const streambuf_type *streambuf_;

    mutex mutex_;

    explicit mutex_deleter(streambuf_type const *s) : streambuf_(s), mutex_() {}

    ~mutex_deleter()
    {
      unique_lock<recursive_mutex> lock(map_mutex);
      auto it = map.find(streambuf_);
      if (it != map.cend())
      {
        GPCL_ASSERT(it->second.lock().get() == &mutex_);
      }
    }
  };
  auto sp_mutex_deleter = make_shared<mutex_deleter>(streambuf);
  shared_ptr<mutex> sp(sp_mutex_deleter, &sp_mutex_deleter->mutex_);

#if defined GPCL_POSIX
  unique_lock<mutex> lock_dummy(*sp); // ensure the mutex is initialized
#endif
  wp = sp;
  return sp;
}

} // namespace detail

/// basic_syncbuf is a synchronized wrapper for a @ref std::basic_streambuf.
template <typename CharType, typename Traits = std::char_traits<CharType>,
          typename Allocator = std::allocator<CharType>>
class basic_syncbuf : public std::basic_streambuf<CharType, Traits>
{
public:
  using allocator_type = Allocator;
  using streambuf_type = std::basic_streambuf<CharType, Traits>;

private:
  // state flags.
  enum flags
  {
    none = 0,

    emit_on_sync = 1 << 0,
    pending_flush = 1 << 1,
  };

  // the wrapped streambuf.
  std::basic_streambuf<CharType, Traits> *wrapped_ = nullptr;

  unsigned char flag_ = none;

  // internal buffer for temporary storage.
  std::basic_string<CharType, Traits, Allocator> buffer_;

  // mutex which protects the wrapped buffer.
  shared_ptr<mutex> mutex_ = detail::get_mutex_for(wrapped_);

public:
  /// Construct a basic_syncbuf with no wrapped streambuf.
  basic_syncbuf() : basic_syncbuf(nullptr) {}

  /// @brief Construct a basic_syncbuf with emit-on-sync policy set to false,
  /// wrapped streambuf set to @c buffer, and using @c alloc as the allocator
  /// for temporary storage.
  explicit basic_syncbuf(streambuf_type *buffer,
                         const Allocator &alloc = Allocator())
      : wrapped_(buffer),
        buffer_(alloc)
  {
  }

  /// Move constructor.
  ///
  /// @post other.get_wrapped() == nullptr.
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

  /// Move assignment.
  ///
  /// @post other.get_wrapped() == nullptr.
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

  /// Swaps two `basic_syncbuf` objects.
  void swap(basic_syncbuf &other) noexcept
  {
    streambuf_type::swap(other);
    gpcl::swap(wrapped_, other.wrapped_);
    gpcl::swap(flag_, other.flag_);
    gpcl::swap(buffer_, other.buffer_);
    gpcl::swap(mutex_, other.mutex_);
  }

  /// Destructor.
  ///
  /// Destroys the basic_syncbuf and emits its internal buffer .
  ~basic_syncbuf()
  {
    GPCL_TRY { emit(); }
    GPCL_CATCH(...) {}
    GPCL_CATCH_END
  }

  ///  Atomically transmits the entire internal buffer to the wrapped streambuf
  ///  .
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
      if (wrapped_->pubsync() != 0)
        return false;
    }
    return buffer_.empty();
  }

  /// Retrieves the wrapped streambuf pointer.
  streambuf_type *get_wrapped() const noexcept { return wrapped_; }

  /// Retrieves the allocator used by this basic_syncbuf.
  allocator_type get_allocator() const noexcept
  {
    return buffer_.get_allocator();
  }

  /// Changes the current emit-on-sync policy.
  void set_emit_on_sync(bool b) noexcept
  {
    if (b)
      flag_ |= emit_on_sync;
    else
      flag_ &= ~emit_on_sync;
  }

protected:
  /// Either emits, or records a pending flush, depending on the current
  /// emit-on-sync policy.
  int sync() override
  {
    flag_ |= pending_flush;

    GPCL_TRY
    {
      if (flag_ & emit_on_sync)
        return emit() ? 0 : -1;
    }
    GPCL_CATCH(...) { return -1; }
    GPCL_CATCH_END
    return 0;
  }

  using int_type = typename Traits::int_type;

  /// Appends a character to the internal buffer.
  int_type overflow(int_type ch = Traits::eof()) override
  {
    GPCL_TRY
    {
      if (Traits::eq_int_type(ch, Traits::eof()))
        return ~Traits::eof();

      buffer_.push_back(ch);
      return ~Traits::eof();
    }
    GPCL_CATCH(...) { return Traits::eof(); }
    GPCL_CATCH_END
    return 0;
  }

  std::streamsize xsputn(const CharType *s,
                         std::streamsize count) noexcept override
  {
    GPCL_TRY
    {
      buffer_.append(s, count);
      return count;
    }
    GPCL_CATCH(...) { return 0; }
    GPCL_CATCH_END
  }
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
