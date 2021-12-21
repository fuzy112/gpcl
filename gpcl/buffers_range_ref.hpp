//
// buffers_range_ref.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BUFFERS_RANGE_REF
#define GPCL_BUFFERS_RANGE_REF

#include <gpcl/buffer_sequence.hpp>

namespace gpcl {
namespace detail {
template <typename BufferSequence>
struct buffers_range_adaptor
{
  const BufferSequence &buffers;

  auto begin() const { return gpcl::buffer_sequence_begin(buffers); }

  auto end() const { return gpcl::buffer_sequence_end(buffers); }
};

struct buffers_range_ref_impl
{
  template <typename BufferSequence,
            std::enable_if_t<
                gpcl::is_const_buffer_sequence<BufferSequence>::value, int> = 0>
  auto operator()(BufferSequence const &bs) const
  {
    return buffers_range_adaptor<BufferSequence>{bs};
  }
};
} // namespace detail

inline constexpr detail::buffers_range_ref_impl
    buffers_range_ref{};

} // namespace gpcl

#endif // GPCL_BUFFERS_RANGE_REF
