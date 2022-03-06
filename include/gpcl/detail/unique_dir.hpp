#ifndef GPCL_DETAIL_UNIQUE_DIR_HPP
#define GPCL_DETAIL_UNIQUE_DIR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>

#include <dirent.h>
#include <sys/types.h>

namespace gpcl { namespace detail {

struct dir_traits
{
  using native_handle_type = DIR *;

  static inline constexpr DIR* invalid_value{nullptr};

  static void close(DIR *dir) noexcept
  {
    if (dir != nullptr)
    {
      closedir(dir);
    }
  }
};

using unique_dir = unique_handle<dir_traits>;

}}

#endif // GPCL_DETAIL_UNIQUE_DIR_HPP
