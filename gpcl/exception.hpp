//
// exception.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EXCEPTION_HPP
#define GPCL_EXCEPTION_HPP

#include <gpcl/debugstream.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/error_info.hpp>
#include <gpcl/excfwd.hpp>
#include <gpcl/intrusive_list.hpp>

#include <iomanip>

namespace gpcl {

class exception
{
  template <typename E>
  friend std::string diagnostic_information(E const &exc);

  intrusive_list<detail::error_info_base> error_info_list_;

  void release() noexcept
  {
    while (!error_info_list_.empty())
    {
      auto &ei = error_info_list_.back();
      error_info_list_.pop_back();
      intrusive_ref_count_dec(ei);
    }
  }

public:
  exception() noexcept {}

  exception(const exception &) = delete;
  exception &operator=(const exception &) = delete;

  exception(exception &&other) noexcept
      : error_info_list_(std::move(other.error_info_list_))
  {
  }

  exception &operator=(exception &&other) noexcept
  {
    error_info_list_ = std::move(other.error_info_list_);
    return *this;
  }

  virtual ~exception() { release(); }

  template <typename Tag, typename T>
  void add_error_info(error_info<Tag, T> &&info) noexcept
  {
    auto &ei = *info.impl_;
    info.impl_.release();
    error_info_list_.push_back(ei);
  }

  template <typename E, typename Tag, typename T>
  friend E &&operator<<(E &&e, error_info<Tag, T> &&info) noexcept
  {
    e.add_error_info(std::move(info));
    return std::forward<E>(e);
  }

  template <typename ErrorInfo>
  friend typename ErrorInfo::value_type const *
  get_error_info(exception const &exc) noexcept
  {
    for (const detail::error_info_base &ei : exc.error_info_list_)
    {
      if (ei.type() == typeid_<ErrorInfo *>())
        return std::addressof(
            static_pointer_cast<const typename ErrorInfo::impl_type>(&ei)
                ->value());
    }
    return nullptr;
  }
};

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

using source_file_errinfo =
    error_info<struct source_file_errinfo_, const char *>;
using source_line_errinfo = error_info<struct source_line_errinfo_, unsigned>;
using func_name_errinfo = error_info<struct func_name_errinfo_, const char *>;

inline std::string to_string(source_file_errinfo const &ei)
{
  std::ostringstream oss;
  oss << "[source_file_errinfo] = " << std::quoted(ei.value());
  return oss.str();
}

inline std::string to_string(source_line_errinfo const &ei)
{
  std::ostringstream oss;
  oss << "[source_line_errinfo] = " << ei.value();
  return oss.str();
}

inline std::string to_string(func_name_errinfo const &ei)
{
  std::ostringstream oss;
  oss << "[func_name_errinfo] = " << std::quoted(ei.value());
  return oss.str();
}

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

template <typename CharT, typename Traits, typename E,
          typename Enable = typename std::enable_if<
              std::is_base_of<exception, E>::value>::type>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &out, const E &exc)
{
  return out << diagnostic_information(exc);
}

template <typename E,
          typename std::enable_if<std::is_base_of<std::exception, E>::value,
                                  int>::type = 0>
const char *exception_name(const E &e) noexcept
{
  return e.what();
}

template <typename E,
          typename std::enable_if<!std::is_base_of<std::exception, E>::value,
                                  int>::type = 0>
const char *exception_name(const E &) noexcept
{
  return typeid_<E>().name();
}

template <typename E>
std::string diagnostic_information(E const &exc)
{
  std::ostringstream oss;
  oss << "Exception [" << exception_name(exc) << "]\n";

  gpcl::exception const *exc_ = dyn_cast<gpcl::exception>(&exc);
  if (!exc_)
    return oss.str();

  for (const detail::error_info_base &ei : exc_->error_info_list_)
  {
    oss << "  ";
    ei.format_to(oss) << "\n";
  }

  return oss.str();
}

template <typename E>
[[noreturn]] void throw_exception(E &&e)
{
#if defined GPCL_NO_EXCEPTIONS
  cdebug() << "Trying to an exception, but exception support is disabled.\n"
           << enable_error_info(e) << "\nTracing back:\n"
           << stacktrace::current() << "\nTerminating..." << std::endl;
#endif
  GPCL_THROW(enable_error_info(std::move(e)));
}

} // namespace gpcl

#define GPCL_THROW_EXCEPTION(exc)                                              \
  ::gpcl::throw_exception(::gpcl::enable_error_info(exc)                       \
                          << ::gpcl::source_file_errinfo(__FILE__)             \
                          << ::gpcl::source_line_errinfo(__LINE__)             \
                          << ::gpcl::func_name_errinfo(__func__))

#endif // GPCL_EXCEPTION_HPP
