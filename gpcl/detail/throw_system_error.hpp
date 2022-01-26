#pragma once

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>

#include <exception>

namespace gpcl {

template <typename E>
[[noreturn]] void throw_exception(E &&e);

#if defined(__clang__)
#  define GPCL_NORETURN_UNLESS_CLANG
#else
#  define GPCL_NORETURN_UNLESS_CLANG [[noreturn]]
#endif

extern template GPCL_NORETURN_UNLESS_CLANG GPCL_EXPORT_DECL void
throw_exception(detail::system_error &&e);
extern template GPCL_NORETURN_UNLESS_CLANG GPCL_EXPORT_DECL void
throw_exception(detail::system_error &e);

namespace detail {

inline namespace errors {

class interrupted : public std::exception
{
public:
  interrupted() = default;
  czstring<> what() const noexcept final { return "thread interrupted"; }
};
} // namespace errors

#if defined(GPCL_POSIX)
[[noreturn]] GPCL_DECL void throw_system_error(int err, czstring<> what);
#elif defined(GPCL_WINDOWS)
[[noreturn]] GPCL_DECL void throw_system_error(DWORD err, czstring<> what);
#endif

[[noreturn]] inline void throw_system_error(czstring<> what)
{
#if defined(GPCL_WINDOWS)
  throw_system_error(GetLastError(), what);
#elif defined(GPCL_POSIX)
  throw_system_error(errno, what);
#endif
}

template <typename Errc, typename std::enable_if<!std::is_integral<Errc>::value,
                                                 int>::type = 0>
[[noreturn]] inline void throw_system_error(Errc errc, czstring<> what)
{
  throw_exception(system_error(make_error_code(errc), what));
}

} // namespace detail
} // namespace gpcl
