//
// win_file.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_FILE_HPP
#define GPCL_DETAIL_WIN_FILE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/creation_tag.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/zstring.hpp>

#include <winnt.h>

namespace gpcl {
namespace detail {

class win_file
{
public:
  typedef DWORD offset_type;

  typedef HANDLE native_handle_type;

  enum access_mode
  {
    readonly = GENERIC_READ,
    writeonly = GENERIC_WRITE,
    readwrite = GENERIC_READ | GENERIC_WRITE,
  };

  constexpr win_file() = default;

  explicit constexpr win_file(native_handle_type fd) : hd_(fd) {}

  template <typename CreationTag>
  win_file(CreationTag, cwzstring<> filename, access_mode mode)
  {
    open(CreationTag{}, filename, mode);
  }

  win_file(win_file &&other) noexcept
      : hd_(exchange(other.hd_, INVALID_HANDLE_VALUE))
  {
  }

  win_file &operator=(win_file &&other) noexcept
  {
    swap(other);
    return *this;
  }

  ~win_file() noexcept { close(); }

  void swap(win_file &other) noexcept
  {
    using gpcl::swap;
    swap(hd_, other.hd_);
  }

  native_handle_type native_handle() const { return hd_; }

  void assign(native_handle_type fd)
  {
    close();
    hd_ = fd;
  }

  native_handle_type release() { return exchange(hd_, INVALID_HANDLE_VALUE); }

  GPCL_DECL void open(open_only_t, cwzstring<> filename, access_mode mode,
                      error_code &error);

  GPCL_DECL void open(create_only_t, cwzstring<> filename, access_mode mode,
                      error_code &error);

  GPCL_DECL void open(open_or_create_t, cwzstring<> filename, access_mode mode,
                      error_code &error);

  template <typename CreationTag>
  void open(CreationTag tag, cwzstring<> filename, access_mode mode)
  {
    error_code ec;
    open(tag, filename, mode, ec);
    if (ec)
      GPCL_THROW(system_error(ec, __func__));
  }

  GPCL_DECL void close(error_code &error);

  void close()
  {
    error_code ec;
    close(ec);
    if (ec)
    {
      GPCL_FATAL(ec.value());
    }
  }

  GPCL_DECL win_file clone(error_code &error);

  win_file clone()
  {
    error_code ec;
    auto ret = clone(ec);
    if (ec)
    {
      GPCL_FATAL(ec.value());
    }
    return ret;
  }

  bool is_open() const noexcept { return INVALID_HANDLE_VALUE != hd_; }

  template <typename ConstBufferSequence>
  std::size_t write_some_at(offset_type off, const ConstBufferSequence &bs,
                            error_code &error);

  template <typename ConstBufferSequence>
  std::size_t write_some_at(offset_type off, const ConstBufferSequence &bs)
  {
    error_code error;
    auto ret = write_some_at(off, bs, error);
    if (error)
      GPCL_THROW(system_error(error, __func__));
    return ret;
  }

  template <typename MutableBufferSequence>
  std::size_t read_some_at(offset_type off, const MutableBufferSequence &bs,
                           error_code &error);

  template <typename MutableBufferSequence>
  std::size_t read_some_at(offset_type off, const MutableBufferSequence &bs)
  {
    error_code error;
    auto ret = read_some_at(off, bs);
    if (error)
      GPCL_THROW(system_error(error, __func__));
    return ret;
  }

  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence &bs, error_code &error)
  {
    return write_some_at(0, bs, error);
  }

  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence &bs)
  {
    error_code error;
    auto ret = write_some(bs, error);
    if (error)
      GPCL_THROW(system_error(error, __func__));
    return ret;
  }

  template <typename MutableBufferSequence>
  std::size_t read_some_at(const MutableBufferSequence &bs, error_code &error)
  {
    return read_some_at(0, bs, error);
  }

  template <typename MutableBufferSequence>
  std::size_t read_some_at(const MutableBufferSequence &bs)
  {
    error_code error;
    auto ret = read_some(bs, error);
    if (error)
      GPCL_THROW(system_error(error, __func__));
    return ret;
  }

  static GPCL_DECL void unlink(cwzstring<> filename, error_code &error);

  static bool unlink(cwzstring<> filename)
  {
    error_code error;
    unlink(filename, error);
    return !error;
  }

private:
  native_handle_type hd_{INVALID_HANDLE_VALUE};
};

inline void swap(win_file &x, win_file &y) noexcept
{
  x.swap(y);
}

} // namespace detail
} // namespace gpcl

#endif // !GPCL_DETAIL_WIN_FILE_HPP
