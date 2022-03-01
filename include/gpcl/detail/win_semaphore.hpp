//
// win_semaphore.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_SEMAPHORE_HPP
#define GPCL_DETAIL_WIN_SEMAPHORE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>
#include <gpcl/noncopyable.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>

#include <winnt.h>

namespace gpcl {
namespace detail {

class win_semaphore : noncopyable
{
public:
  using value_type = unsigned int;

  GPCL_DECL explicit win_semaphore(value_type init_value);
  ~win_semaphore() = default;

  GPCL_DECL auto try_wait() -> bool;
  GPCL_DECL auto wait() -> void;
  GPCL_DECL auto post() -> void;

#undef max
  static constexpr std::ptrdiff_t max() noexcept
  {
    return (std::numeric_limits<LONG>::max)();
  }

  using native_handle_type = HANDLE;
  auto native_handle() -> native_handle_type { return sem_.get(); }

private:
  nullable_handle sem_;
};
} // namespace detail
} // namespace gpcl

#endif
