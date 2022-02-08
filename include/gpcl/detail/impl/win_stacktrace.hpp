//
// win_stacktrace.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_STACKTRACE_HPP
#define GPCL_DETAIL_IMPL_WIN_STACKTRACE_HPP

#include <gpcl/detail/win_stacktrace.hpp>
#include <gpcl/unique_lock.hpp>

#include <winnt.h>

namespace gpcl::detail {
template <typename Allocator>
DECLSPEC_NOINLINE void win_stacktrace_impl(
    std::size_t skip, std::size_t max_depth,
    std::vector<win_stacktrace_entry, Allocator> &container) noexcept
{
  GPCL_TRY
  {
    std::vector<PVOID, typename std::allocator_traits<
                           Allocator>::template rebind_alloc<PVOID>>
        buffer(container.get_allocator());

    if (max_depth == std::size_t(-1))
      max_depth = 63;

    buffer.resize(max_depth);

    ULONG hash = 0;
    USHORT n = RtlCaptureStackBackTrace((DWORD)skip + 1, (DWORD)buffer.size(),
                                        buffer.data(), &hash);
    buffer.resize(n);

    container.reserve(buffer.size());
    for (auto addr : buffer)
    {
      container.emplace_back(addr);
    }
  }
  GPCL_CATCH(...) {}
  GPCL_CATCH_END
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_WIN_STACKTRACE_HPP
