//
// mutable_buffer.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MUTABLE_BUFFER_HPP
#define GPCL_MUTABLE_BUFFER_HPP

#include <gpcl/span.hpp>

namespace gpcl
{
    
/// Mutable buffer.
using mutable_buffer = span<unsigned char>;

/// Consume some bytes of the buffer.
inline mutable_buffer &operator+=(mutable_buffer &buf, std::size_t sz)
{
  return buf = buf.subspan(sz);
}

}

#endif // GPCL_MUTABLE_BUFFER_HPP
