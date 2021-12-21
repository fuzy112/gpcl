//
// buffer_size.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BUFFER_SIZE_HPP
#define GPCL_BUFFER_SIZE_HPP

#include <gpcl/buffer.hpp>
#include <gpcl/buffers_range_ref.hpp>

namespace gpcl {
namespace detail {

inline std::size_t buffer_size(const_buffer b) noexcept
{
  return b.size();
}

template <typename BufferSequence>
std::size_t buffer_size(const BufferSequence &bs) noexcept
{
  std::size_t total_bytes = 0;
  for (auto b : gpcl::buffers_range_ref(bs))
  {
    total_bytes += b.size();
  }
  return total_bytes;
}

struct buffer_size_impl
{
  template <typename BufferSequence>
  constexpr std::size_t operator()(const BufferSequence &bs) const noexcept
  {
    return buffer_size(bs);
  }
};

} // namespace detail

inline constexpr detail::buffer_size_impl buffer_size{};

} // namespace gpcl

#endif
