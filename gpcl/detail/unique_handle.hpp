//
// unique_handle.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#include <gpcl/detail/config.hpp>

#include <gpcl/unique_resource.hpp>

#ifdef GPCL_WINDOWS
#  include <windows.h>

namespace gpcl {

namespace detail {

struct invalid_handle_deleter
{
  using pointer = ::HANDLE;

  static auto invalid() noexcept -> pointer { return INVALID_HANDLE_VALUE; }

  GPCL_DECL auto operator()(pointer h) noexcept -> void;
};

struct null_handle_deleter
{
  using pointer = ::HANDLE;

  static constexpr auto invalid() noexcept -> ::HANDLE { return nullptr; }

  GPCL_DECL auto operator()(::HANDLE h) noexcept -> void;
};

using invalid_handle = unique_resource<::HANDLE, invalid_handle_deleter>;

using null_handle = unique_resource<::HANDLE, null_handle_deleter>;

} // namespace detail

} // namespace gpcl

#  ifdef GPCL_HEADER_ONLY
#    include <gpcl/detail/impl/unique_handle.ipp>
#  endif

#endif
