//
// posix_file.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_FILE_HPP
#define GPCL_DETAIL_POSIX_FILE_HPP

#include <gpcl/buffer_sequence.hpp>
#include <gpcl/creation_tag.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/error.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/zstring.hpp>

#include <fcntl.h>

namespace gpcl {
namespace detail {

class posix_file
{
public:
  typedef off64_t offset_type;

  typedef int native_handle_type;

  enum access_mode
  {
    readonly = O_RDONLY,
    writeonly = O_WRONLY,
    readwrite = O_RDWR,
  };

  enum seek_direction
  {
    seek_begin = SEEK_SET,
    seek_current = SEEK_CUR,
    seek_end = SEEK_END,
  };

  constexpr posix_file() = default;

  explicit constexpr posix_file(native_handle_type fd) : fd_(fd) {}

  template <typename CreationTag>
  posix_file(CreationTag, czstring<> filename, access_mode mode)
  {
    open(CreationTag{}, filename, mode);
  }

  posix_file(posix_file &&other) noexcept : fd_(exchange(other.fd_, -1)) {}

  posix_file &operator=(posix_file &&other) noexcept
  {
    swap(other);
    return *this;
  }

  ~posix_file() noexcept { close(); }

  void swap(posix_file &other) noexcept
  {
    using std::swap;
    swap(fd_, other.fd_);
  }

  native_handle_type native_handle() const { return fd_; }

  void assign(native_handle_type fd)
  {
    close();
    fd_ = fd;
  }

  native_handle_type release() { return exchange(fd_, -1); }

  GPCL_DECL void open(open_only_t, czstring<> filename, access_mode mode,
                      error_code &error);

  GPCL_DECL void open(create_only_t, czstring<> filename, access_mode mode,
                      error_code &error);

  GPCL_DECL void open(open_or_create_t, czstring<> filename, access_mode mode,
                      error_code &error);

  template <typename CreationTag>
  void open(CreationTag tag, czstring<> filename, access_mode mode)
  {
    error_code ec;
    posix_file::open(tag, filename, mode, ec);
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

  GPCL_DECL posix_file clone(error_code &error);

  posix_file clone()
  {
    error_code ec;
    auto ret = clone(ec);
    if (ec)
    {
      GPCL_FATAL(ec.value());
    }
    return ret;
  }

  constexpr bool is_open() const noexcept { return -1 != fd_; }

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
    auto ret = read_some_at(off, bs, error);
    if (error)
      GPCL_THROW(system_error(error, __func__));
    return ret;
  }

  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence &bs, error_code &error);

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
  std::size_t read_some(const MutableBufferSequence &bs, error_code &error);

  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence &bs)
  {
    error_code error;
    auto ret = read_some(bs, error);
    if (error)
      GPCL_THROW(system_error(error, __func__));
    return ret;
  }

  GPCL_DECL std::size_t tell(error_code &error);

  std::size_t tell()
  {
    error_code error;
    std::size_t result = tell(error);
    if (error)
      GPCL_THROW((system_error{error, __func__}));
    return result;
  }

  GPCL_DECL void seek(offset_type off, seek_direction dir, error_code &error);

  void seek(offset_type off, seek_direction dir)
  {
    error_code error;
    seek(off, dir, error);
    if (error)
      GPCL_THROW((system_error{error, __func__}));
  }

  void seek(offset_type pos, error_code &error)
  {
    seek(pos, seek_begin, error);
  }

  void seek(offset_type pos)
  {
    error_code error;
    seek(pos, error);
    if (error)
      GPCL_THROW((system_error{error, __func__}));
  }

  GPCL_DECL void truncate(std::size_t size, error_code &error);

  void truncate(std::size_t size)
  {
    error_code error;
    truncate(size, error);
    if (error)
      GPCL_THROW((system_error{error, __func__}));
  }

  GPCL_DECL static void unlink(czstring<> filename, error_code &error);

  static bool unlink(czstring<> filename)
  {
    error_code error;
    posix_file::unlink(filename, error);
    return !error;
  }

private:
  native_handle_type fd_{-1};
};

inline void swap(posix_file &x, posix_file &y) noexcept
{
  x.swap(y);
}

} // namespace detail
} // namespace gpcl

#include <gpcl/detail/impl/posix_file.hpp>

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/posix_file.ipp>
#endif // GPCL_HEADER_ONLY

#endif // GPCL_DETAIL_POSIX_FILE_HPP
