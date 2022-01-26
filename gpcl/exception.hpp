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
#include <gpcl/error.hpp>
#include <gpcl/generic_pointer_cast.hpp>
#include <gpcl/intrusive_list.hpp>
#include <gpcl/intrusive_ptr.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/stacktrace.hpp>
#include <gpcl/typeid.hpp>

#include <atomic>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace gpcl {

class exception;

template <typename Tag, typename T>
class error_info;

template <typename E, typename Tag, typename T>
E &&operator<<(E &&e, error_info<Tag, T> &&info) noexcept;

template <typename E>
std::string diagnostic_information(E const &exc);

class error_info_base : public intrusive_list_node<error_info_base>
{
  friend exception;
  template <typename E, typename Tag, typename T>
  friend E &&operator<<(E &&e, error_info<Tag, T> &&info) noexcept;
  template <typename E>
  friend std::string diagnostic_information(E const &exc);

  mutable std::atomic_long ref_count_{1};

protected:
  virtual ~error_info_base()
  {
    if (in_list())
      gpcl::delete_entry(this);
  }

public:
  virtual const type_info &type() const noexcept = 0;

  virtual std::string to_string() const = 0;

  friend inline void
  intrusive_ref_count_inc(const error_info_base *errinfo) noexcept
  {
    errinfo->ref_count_ += 1;
  }

  friend inline void
  intrusive_ref_count_dec(const error_info_base *errinfo) noexcept
  {
    if (errinfo->ref_count_.fetch_sub(1) == 1)
    {
      delete errinfo;
    }
  }
};

template <typename Tag, typename T>
std::string to_string(error_info<Tag, T> const &errinfo);

namespace detail {
template <typename Tag, typename T>
std::string to_string_impl(error_info<Tag, T> const &errinfo)
{
  return to_string(errinfo);
}

template <typename T, decltype(std::declval<std::ostringstream &>()
                                   << std::declval<const T &>(),
                               0) = 0>
std::string to_string(const T &value)
{
  std::ostringstream ss;
  ss << value;
  return ss.str();
}

template <typename T>
std::string translate_error_info_value(const T &value)
{
  return to_string(value);
}

} // namespace detail

template <typename Tag, typename T>
class error_info;

template <typename Tag, typename T>
class error_info_impl : public error_info_base, private noncopyable
{
  friend error_info<Tag, T>;

public:
  using tag = Tag;
  using value_type = T;

private:
  value_type value_;

private:
  template <typename Enable = typename std::enable_if<
                std::is_default_constructible<T>::value>::type>
  error_info_impl() : value_()
  {
  }

  template <typename... Args>
  explicit error_info_impl(Args &&... args)
      : value_(std::forward<Args>(args)...)
  {
  }

  template <typename U, typename... Args>
  explicit error_info_impl(std::initializer_list<U> il, Args &&... args)
      : value_(il, std::forward<Args>(args)...)
  {
  }

protected:
  ~error_info_impl() override {}

public:
  value_type const &value() const { return value_; }

  type_info const &type() const noexcept override
  {
    return typeid_<error_info<Tag, T>>();
  }

  std::string to_string() const override;
};

template <typename Tag, typename T>
class error_info
{
  friend exception;

public:
  using impl_type = error_info_impl<Tag, T>;

  using value_type = T;
  using tag = Tag;

private:
  intrusive_ptr<impl_type> impl_;

public:
  template <typename Enable = typename std::enable_if<
                std::is_default_constructible<T>::value>::type>
  error_info() : impl_(new impl_type())
  {
  }

  template <typename... Args>
  explicit error_info(Args &&... args)
      : impl_(new impl_type(std::forward<Args>(args)...))
  {
  }

  template <typename U, typename... Args>
  explicit error_info(std::initializer_list<U> il, Args &&... args)
      : impl_(new impl_type(il, std::forward<Args>(args)...))
  {
  }

  explicit error_info(impl_type *impl) noexcept : impl_(impl, true) {}

  explicit error_info(const impl_type *impl) = delete;

  error_info(const error_info &) = default;
  error_info(error_info &&) noexcept = default;
  error_info &operator=(const error_info &) = default;
  error_info &operator=(error_info &&) noexcept = default;

  value_type const &value() const { return impl_->value(); }

  type_info const &type() const noexcept { return typeid_<error_info>(); }
};

template <typename Tag, typename T>
std::string error_info_impl<Tag, T>::to_string() const
{
  return detail::to_string_impl(
      error_info<Tag, T>(const_cast<error_info_impl *>(this)));
}

template <typename Tag, typename T>
std::string to_string(error_info<Tag, T> const &errinfo)
{
  std::ostringstream oss;
  oss << "[" << typeid_<Tag *>().name() << "] = { "
      << detail::translate_error_info_value(errinfo.value()) << " }";
  return oss.str();
}

template <typename CharT, typename Traits, typename Tag, typename T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &out,
           error_info<Tag, T> const &errinfo)
{
  return out << to_string(errinfo);
}

class exception
{
  template <typename E>
  friend std::string diagnostic_information(E const &exc);

  intrusive_list<error_info_base> error_info_list_;

  void release() noexcept
  {
    while (!error_info_list_.empty())
    {
      auto &ei = error_info_list_.back();
      error_info_list_.pop_back();
      intrusive_ref_count_dec(&ei);
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

  // void swap(exception &other) noexcept
  // {
  //   using gpcl::swap;

  //   swap(error_info_list_, other.error_info_list_);
  // }

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
    for (const error_info_base &ei : exc.error_info_list_)
    {
      if (ei.type() == typeid_<ErrorInfo>())
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

  for (const error_info_base &ei : exc_->error_info_list_)
  {
    oss << "  " << ei.to_string() << "\n";
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
