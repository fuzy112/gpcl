#ifndef GPCL_CONST_BUFFER_HPP
#define GPCL_CONST_BUFFER_HPP

#include <gpcl/span.hpp>

namespace gpcl
{
    /// Constant buffer.
using const_buffer = span<const unsigned char>;


/// Consume some bytes of the buffer.
inline const_buffer &operator+=(const_buffer &buf, std::size_t sz)
{
  return buf = buf.subspan(sz);
}

}

#endif // GPCL_CONST_BUFFER_HPP
