#pragma once

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/strerror.hpp>

namespace gpcl {
inline std::string strerror(int errnum)
{
  std::string str;
  detail::strerror_impl(str, errnum);
  return str;
}

inline void strerror(std::string &str, int errnum)
{
  detail::strerror_impl(str, errnum);
}
} // namespace gpcl
