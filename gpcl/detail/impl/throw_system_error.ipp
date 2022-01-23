#pragma once

#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/exception.hpp>

namespace gpcl {

template [[noreturn]] void
throw_exception(detail::system_error &&e);
template [[noreturn]] void
throw_exception(detail::system_error &e);

} // namespace gpcl

namespace gpcl::detail {

#if defined(GPCL_POSIX)
void throw_system_error(int err, czstring<> what)
{
  if (err == EINTR)
  {
    GPCL_THROW(interrupted());
  }

  throw_exception(system_error(err, generic_category(), what));
}

#elif defined(GPCL_WINDOWS)
[[noreturn]] void throw_system_error(DWORD err, czstring<> what)
{
  throw_exception(system_error(err, system_category(), what));
}
#endif

} // namespace gpcl::detail
