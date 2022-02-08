//
// anonymous_shared_memory.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ANONYMOUS_SHARED_MEMORY_HPP
#define GPCL_ANONYMOUS_SHARED_MEMORY_HPP

#include <gpcl/detail/config.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/anonymous_shared_memory.hpp>
namespace gpcl {
inline auto anonymous_shared_memory(std::size_t size) noexcept
{
  return gpcl::detail::anonymous_shared_memory_impl{size};
}
} // namespace gpcl
#endif

#endif // GPCL_ANONYMOUS_SHARED_MEMORY_HPP
