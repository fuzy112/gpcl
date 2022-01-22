#pragma once

#include <gpcl/exception.hpp>

namespace gpcl {

std::string diagnostic_information(exception const &exc)
{
  std::ostringstream oss;
  if (auto *std_except = dynamic_cast<std::exception const *>(&exc))
  {
    oss << std_except->what() << ":\n";
  }
  else
  {
    oss << typeid(exc).name() << ":\n";
  }

  for (error_info_base *errinfo : exc.error_infos_)
  {
    oss << errinfo->to_string() << "\n";
  }

  return oss.str();
}

} // namespace gpcl
