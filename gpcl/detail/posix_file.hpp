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
#include <gpcl/expected.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/zstring.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
    open(CreationTag{}, filename, mode).value();
  }

  posix_file(posix_file &&other) noexcept : fd_(exchange(other.fd_, -1)) {}

  posix_file &operator=(posix_file &&other) noexcept
  {
    swap(other);
    return *this;
  }

  ~posix_file() noexcept
  {
    close()
        .or_else([](error_code ec) -> expected<void, error_code> {
          if (ec == errc::bad_file_descriptor)
            return {};
          return unexpected(ec);
        })
        .value();
  }

  void swap(posix_file &other) noexcept
  {
    using gpcl::swap;
    swap(fd_, other.fd_);
  }

  native_handle_type native_handle() const { return fd_; }

  expected<void, error_code> assign(native_handle_type fd)
  {
    return close()
        .or_else([](error_code ec) -> expected<void, error_code> {
          if (ec == errc::bad_file_descriptor)
            return {};
          return unexpected(ec);
        })
        .and_then([this, fd]() -> expected<void, error_code> {
          fd_ = fd;
          return {};
        });
  }

  native_handle_type release() { return exchange(fd_, -1); }

  GPCL_DECL expected<void, error_code> open(open_only_t, czstring<> filename,
                                            access_mode mode);

  GPCL_DECL expected<void, error_code> open(create_only_t, czstring<> filename,
                                            access_mode mode);

  GPCL_DECL expected<void, error_code>
  open(open_or_create_t, czstring<> filename, access_mode mode);

  GPCL_DECL expected<void, error_code> close();

  GPCL_DECL expected<posix_file, error_code> clone();

  constexpr bool is_open() const noexcept { return -1 != fd_; }

  template <typename ConstBufferSequence>
  expected<std::size_t, error_code>
  write_some_at(offset_type off, const ConstBufferSequence &bs);

  template <typename MutableBufferSequence>
  expected<std::size_t, error_code>
  read_some_at(offset_type off, const MutableBufferSequence &bs);

  template <typename ConstBufferSequence>
  expected<std::size_t, error_code> write_some(const ConstBufferSequence &bs);

  template <typename MutableBufferSequence>
  expected<std::size_t, error_code> read_some(const MutableBufferSequence &bs);

  GPCL_DECL expected<std::size_t, error_code> tell();

  GPCL_DECL expected<void, error_code> seek(offset_type off,
                                            seek_direction dir);

  expected<void, error_code> seek(offset_type pos)
  {
    return seek(pos, seek_begin);
  }

  GPCL_DECL expected<void, error_code> truncate(std::size_t size);

  GPCL_DECL static expected<void, error_code> unlink(czstring<> filename);

  std::size_t size() const
  {
    struct ::stat s;
    if (-1 == ::fstat(fd_, &s))
      throw_system_error(__func__);
    return s.st_size;
  }

private:
  native_handle_type fd_{-1};
};

namespace swap_detail {
inline void swap(posix_file &x, posix_file &y) noexcept
{
  x.swap(y);
}
} // namespace swap_detail

} // namespace detail
} // namespace gpcl

#include <gpcl/detail/impl/posix_file.hpp>

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/posix_file.ipp>
#endif // GPCL_HEADER_ONLY

#endif // GPCL_DETAIL_POSIX_FILE_HPP
