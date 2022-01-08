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

struct buffer_size_fn
{
  template <
      typename T,
      std::enable_if_t<
          is_const_buffer_sequence<T>::value &&
              std::is_same_v<tag_invoke_result_t<buffer_size_fn, const T &>,
                             std::size_t>,
          int> = 0>
  std::size_t operator()(const T &x) const
  {
    return gpcl::tag_invoke(*this, x);
  }
};

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

template <typename T>
auto tag_invoke(buffer_size_fn, const T &x)
    -> std::enable_if_t<std::is_same_v<decltype(buffer_size(x)), std::size_t>,
                        std::size_t>
{
  return buffer_size(x);
}

} // namespace detail

using buffer_size_fn = detail::buffer_size_fn;

constexpr buffer_size_fn buffer_size{};

} // namespace gpcl

#endif
