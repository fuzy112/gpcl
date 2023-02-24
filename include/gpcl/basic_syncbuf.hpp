//
// basic_syncbuf.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2023 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BASIC_SYNCBUF_HPP
#define GPCL_BASIC_SYNCBUF_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/get_mutex_for_address.hpp>
#include <gpcl/error.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/scoped_lock.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/weak_ptr.hpp>

#include <streambuf>
#include <unordered_map>
#include <bitset>

namespace gpcl {

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
    emit_on_sync,
    pending_flush,

    n_flags,
  };

  // the wrapped streambuf.
  std::basic_streambuf<CharType, Traits> *wrapped_ = nullptr;

  std::bitset<n_flags> flags_;

  // internal buffer for temporary storage.
  std::basic_string<CharType, Traits, Allocator> buffer_;

  // mutex which protects the wrapped buffer.
  detail::mutex_for_address_ptr mutex_ =
      detail::get_mutex_for_address(wrapped_);

public:
  /// Construct a basic_syncbuf with no wrapped streambuf.
  basic_syncbuf() : basic_syncbuf(nullptr) {}

  /// @brief Construct a basic_syncbuf with emit-on-sync policy set to false,
  /// wrapped streambuf set to @c buffer, and using @c alloc as the allocator
  /// for temporary storage.
  explicit basic_syncbuf(streambuf_type *streambuf,
                         const Allocator &alloc = Allocator())
      : wrapped_(streambuf),
        buffer_(alloc)
  {
  }

  /// Move constructor.
  ///
  /// @post other.get_wrapped() == nullptr.
  basic_syncbuf(basic_syncbuf &&other) noexcept
      : streambuf_type(other),
        wrapped_(other.wrapped_),
        flags_(other.flags_),
        buffer_(std::move(other).buffer_),
        mutex_(std::move(other).mutex_)
  {
    other.wrapped_ = false;
    other.flags_.reset();
    other.mutex_ = nullptr;
  }

  /// Move assignment.
  ///
  /// @post other.get_wrapped() == nullptr.
  basic_syncbuf &operator=(basic_syncbuf &&other) noexcept
  {
    emit();

    streambuf_type::operator=(std::move(other));
    wrapped_ = other.wrapped_;
    flags_ = other.flags_;
    buffer_ = std::move(other).buffer_;
    mutex_ = std::move(other).mutex_;

    other.wrapped_ = nullptr;
    other.flags_.reset();
    other.mutex_ = nullptr;

    return *this;
  }

  /// Swaps two `basic_syncbuf` objects.
  void swap(basic_syncbuf &other) noexcept
  {
    streambuf_type::swap(other);
    swap(wrapped_, other.wrapped_);
    swap(flags_, other.flags_);
    swap(buffer_, other.buffer_);
    swap(mutex_, other.mutex_);
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

    gpcl::scoped_lock<mutex> lock(*mutex_);
    auto n = wrapped_->sputn(buffer_.data(), buffer_.size());
    buffer_.erase(buffer_.begin(), buffer_.begin() + n);

    if (flags_.test(pending_flush))
    {
      flags_.reset(pending_flush);
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
    flags_.set(emit_on_sync, b);
  }

protected:
  /// Either emits, or records a pending flush, depending on the current
  /// emit-on-sync policy.
  int sync() override
  {
    flags_.set(pending_flush);

    GPCL_TRY
    {
      if (flags_.test(emit_on_sync))
        return emit() ? 0 : -1;
    }
    GPCL_CATCH(...) { return -1; }
    GPCL_CATCH_END
    return 0;
  }

  using int_type = typename Traits::int_type;

  GPCL_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wconstant-conversion")

  /// Appends a character to the internal buffer.
  int_type overflow(int_type ch = Traits::eof()) override
  {
    GPCL_TRY
    {
      if (Traits::eq_int_type(ch, Traits::eof()))
        return ~Traits::eof();

      buffer_.push_back(Traits::to_char_type(ch));
      return ~Traits::eof();
    }
    GPCL_CATCH(...) { return Traits::eof(); }
    GPCL_CATCH_END
    return 0;
  }

  GPCL_CLANG_SUPPRESS_WARNING_POP

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

template <typename CharType, typename Traits, typename Allocator>
void swap(basic_syncbuf<CharType, Traits, Allocator> &x,
          basic_syncbuf<CharType, Traits, Allocator> &y) noexcept
{
  x.swap(y);
}

using syncbuf = basic_syncbuf<char>;
using wsyncbuf = basic_syncbuf<wchar_t>;

} // namespace gpcl

#endif // GPCL_BASIC_SYNCBUF_HPP
