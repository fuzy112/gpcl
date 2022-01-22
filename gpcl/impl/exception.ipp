#pragma once

#include <gpcl/exception.hpp>

namespace gpcl {

std::string diagnostic_information(exception const &exc)
{
  std::ostringstream oss;
#if defined(GPCL_NO_RTTI)
  oss << "Unknown exception:\n";
#else
  if (auto *std_except = dynamic_cast<std::exception const *>(&exc))
  {
    oss << std_except->what() << ":\n";
  }
  else
  {
    oss << typeid(exc).name() << ":\n";
  }
#endif

  for (error_info_base *ei = exc.error_infos_; ei != nullptr; ei = ei->next_)
  {
    oss << "  " << ei->to_string() << "\n";
  }

  return oss.str();
}

} // namespace gpcl
