#pragma once

#include <gpcl/detail/config.hpp>

#include <string>

namespace gpcl::detail {

GPCL_DECL
void strerror_impl(std::string &str, int errnum);
} // namespace gpcl::detail
  