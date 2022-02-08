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

/// A range-like view to a buffer sequence.
template <typename BufferSequence>
struct buffers_range_ref
{
  /// Reference to the buffer sequence.
  const BufferSequence &buffers;

  template <typename Buffers = BufferSequence,
       typename Enable = std::enable_if_t<
          is_const_buffer_sequence<Buffers>::value>>
  explicit buffers_range_ref(BufferSequence const &buffers)
      : buffers(buffers)
  {
  }

  /// Returns an iterator to the first buffer.
  auto begin() const { return gpcl::buffer_sequence_begin(buffers); }

  /// Returns an iterator past-the-end of the last buffer.
  auto end() const { return gpcl::buffer_sequence_end(buffers); }
};

} // namespace gpcl

#endif // GPCL_BUFFERS_RANGE_REF
