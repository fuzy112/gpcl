#ifndef GPCL_THROW_EXCEPTION_HPP
#define GPCL_THROW_EXCEPTION_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/exception.hpp>
#include <gpcl/make_iomanip.hpp>

#include <iomanip>

namespace gpcl {

template <typename E>
class wrapped_exception : virtual public E, virtual public exception
{
  static_assert(!std::is_base_of<gpcl::exception, E>::value, "");

public:
  template <typename T>
  explicit wrapped_exception(T &&e) : E(std::forward<T>(e))
  {
  }
};

template <typename E,
          typename std::enable_if<
              std::is_base_of<exception, typename std::decay<E>::type>::value,
              int>::type = 0>
E enable_error_info(E &&e) noexcept
{
  return std::forward<E>(e);
}

template <typename E,
          typename std::enable_if<
              !std::is_base_of<exception, typename std::decay<E>::type>::value,
              int>::type = 0>
wrapped_exception<typename std::decay<E>::type> enable_error_info(E &&e)
{
  return wrapped_exception<typename std::decay<E>::type>(std::forward<E>(e));
}

namespace detail {
using source_file_errinfo =
    error_info<struct source_file_errinfo_, const char *>;
using source_line_errinfo = error_info<struct source_line_errinfo_, unsigned>;
using func_name_errinfo = error_info<struct func_name_errinfo_, const char *>;
} // namespace detail

using detail::func_name_errinfo;
using detail::source_file_errinfo;
using detail::source_line_errinfo;

namespace detail {
inline decltype(auto) tag_invoke(source_file_errinfo::format_fn,
                                 const char *source_file)
{
  return make_iomanip(
      [=](auto &s) { s << "[source file] = " << std::quoted(source_file); });
}

inline decltype(auto) tag_invoke(source_line_errinfo::format_fn, unsigned line)
{
  return make_iomanip([=](auto &s) { s << "[source line] = " << line; });
}

inline decltype(auto) tag_invoke(func_name_errinfo::format_fn,
                                 const char *func_name)
{
  return make_iomanip([=](auto &s) { s << "[function name] = " << func_name; });
}
} // namespace detail

template <typename E>
[[noreturn]] void throw_exception(E &&e)
{
#if defined GPCL_NO_EXCEPTIONS
  cdebug() << "Trying to an exception, but exception support is disabled.\n"
           << enable_error_info(e) << "\nTracing back:\n"
           << stacktrace::current() << "\nTerminating..." << std::endl;
#endif
  GPCL_THROW(enable_error_info(std::forward<E>(e)));
}

#define GPCL_THROW_EXCEPTION(exc)                                              \
  ::gpcl::throw_exception(::gpcl::enable_error_info(exc)                       \
                          << ::gpcl::source_file_errinfo(__FILE__)             \
                          << ::gpcl::source_line_errinfo(__LINE__)             \
                          << ::gpcl::func_name_errinfo(__func__))

} // namespace gpcl

#endif // GPCL_THROW_EXCEPTION_HPP
