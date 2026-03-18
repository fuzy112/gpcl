//
// unique_dir.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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

  static constexpr bool is_valid(native_handle_type h) noexcept
  {
    return !!h;
  }

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
