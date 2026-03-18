//
// buffer_to_string.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BUFFER_TO_STRING_HPP
#define GPCL_BUFFER_TO_STRING_HPP

#include <gpcl/buffer.hpp>
#include <gpcl/buffer_size.hpp>
#include <gpcl/buffers_range_ref.hpp>

#include <string>

namespace gpcl {

template <typename BufferSequence>
std::string buffer_to_string(const BufferSequence &buffers)
{
  static_assert(is_const_buffer_sequence<BufferSequence>::value);
  std::string result;
  result.reserve(buffer_size(buffers));
  for (auto const buffer : buffers_range_ref(buffers))
    result.append(static_cast<const char *>(buffer.data()), buffer.size());
  return result;
}

} // namespace gpcl

#endif // GPCL_BUFFER_TO_STRING_HPP
