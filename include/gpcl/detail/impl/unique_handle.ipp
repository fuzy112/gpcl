//
// unique_handle.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_UNIQUE_HANDLE_IPP
#define GPCL_DETAIL_IMPL_UNIQUE_HANDLE_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unique_handle.hpp>

namespace gpcl {
namespace detail {

auto null_handle_deleter::operator()(pointer h) noexcept -> void
{
  GPCL_VERIFY(::CloseHandle(h));
}

auto invalid_handle_deleter::operator()(pointer h) noexcept -> void
{
  GPCL_VERIFY(::CloseHandle(h));
}

} // namespace detail
} // namespace gpcl


#endif // GPCL_DETAIL_IMPL_UNIQUE_HANDLE_IPP
