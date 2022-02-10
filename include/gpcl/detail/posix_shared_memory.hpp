//
// posix_shared_memory.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_SHARED_MEMORY_HPP
#define GPCL_DETAIL_POSIX_SHARED_MEMORY_HPP

#include <gpcl/access_mode.hpp>
#include <gpcl/creation_tag.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/unique_resource.hpp>
#include <gpcl/zstring.hpp>

#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace gpcl::detail {

class posix_shared_memory;

class posix_shared_memory
{
public:
  struct factory;

  posix_shared_memory() noexcept = default;

  template <typename CreationTag>
  posix_shared_memory(CreationTag tag, czstring<> name, access_mode access,
                      mode_t mode = S_IRUSR | S_IWUSR)
  {
    error_code error;
    open(tag, name, access, mode, error);
    if (error)
      GPCL_THROW(system_error(error, __func__));
  }

  static error_code remove(czstring<> name)
  {
    return {::shm_unlink(name), system_category()};
  }

  using native_handle_type = int;

  native_handle_type native_handle() const { return fd_.get(); }

  std::size_t size() const
  {
    struct ::stat s;
    if (-1 == ::fstat(fd_.get(), &s))
      throw_system_error(__func__);
    return s.st_size;
  }

  void truncate(std::size_t size, error_code &error)
  {
    if (::ftruncate(fd_.get(), size) == -1)
      error = {errno, system_category()};
    else
      error = {};
  }

  void truncate(std::size_t size)
  {
    error_code error;
    truncate(size, error);
    if (error)
      GPCL_THROW(system_error{error, __func__});
  }

private:
  void open(open_only_t, czstring<> name, access_mode access, error_code &error)
  {
    if (access == access_mode::read_write)
      open(name, O_RDWR, 0, error);
    else
      open(name, O_RDONLY, 0, error);
  }

  void open(open_or_create_t, czstring<> name, access_mode access, mode_t mode,
            error_code &error)
  {
    if (access == access_mode::read_write)
      open(name, O_CREAT | O_RDWR, mode, error);
    else
      open(name, O_CREAT | O_RDONLY, mode, error);
  }

  void open(create_only_t, czstring<> name, access_mode access, mode_t mode,
            error_code &error)
  {
    if (access == access_mode::read_write)
      open(name, O_CREAT | O_EXCL | O_RDWR, mode, error);
    else
      open(name, O_CREAT | O_EXCL | O_RDONLY, mode, error);
  }

  void open(czstring<> name, int oflag, ::mode_t mode, error_code &error)
  {
    GPCL_ASSERT(name);
    GPCL_ASSERT(::strlen(name) < NAME_MAX);
    GPCL_ASSERT(name[0] == '/');

    fd_ = make_unique_resource_checked<int, fd_deleter>(
        ::shm_open(name, oflag, mode), -1, fd_deleter{});
    if (!fd_)
      error = {errno, system_category()};
    else
      error = {};
  }

  struct fd_deleter
  {
    void operator()(int fd) const { GPCL_VERIFY_0(::close(fd)); }
  };

  unique_resource<int, fd_deleter> fd_;
};

struct posix_shared_memory::factory
{
  template <typename CreationTag>
  posix_shared_memory operator()(CreationTag t, czstring<> name,
                                 access_mode access, mode_t mode,
                                 error_code &error) const
  {
    posix_shared_memory memory;
    memory.open(t, name, access, mode, error);
    return memory;
  }

  template <typename CreationTag>
  posix_shared_memory operator()(CreationTag t, czstring<> name,
                                 access_mode access, error_code &error) const
  {
    return make_posix_shared_memory(t, name, access, S_IRUSR | S_IWUSR, error);
  }
};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_POSIX_SHARED_MEMORY_HPP
