#ifndef GPCL_EXCEPTION_HPP
#define GPCL_EXCEPTION_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/typeid.hpp>

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
E &&operator<<(E &&e, error_info<Tag, T> &&info);

GPCL_DECL
std::string diagnostic_information(exception const &exc);

class error_info_base
{
  friend exception;
  template <typename E, typename Tag, typename T>
  friend E &&operator<<(E &&e, error_info<Tag, T> &&info);
  friend GPCL_DECL std::string diagnostic_information(exception const &exc);

  error_info_base *next_ = nullptr;

public:
  virtual ~error_info_base() = default;

  virtual const type_info &type() const noexcept = 0;

  virtual std::string to_string() const = 0;
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
class error_info : public error_info_base
{
public:
  using tag = Tag;
  using value_type = T;

private:
  value_type value_;

public:
  template <typename Enable = typename std::enable_if<
                std::is_default_constructible<T>::value>::type>
  error_info() : value_()
  {
  }

  error_info(error_info &&other) noexcept(
      std::is_nothrow_move_constructible<T>::value)
      : value_(std::move_if_noexcept(other.value_))
  {
  }

  template <typename... Args>
  explicit error_info(Args &&...args) : value_(std::forward<Args>(args)...)
  {
  }

  template <typename U, typename... Args>
  explicit error_info(std::initializer_list<U> il, Args &&...args)
      : value_(il, std::forward<Args>(args)...)
  {
  }

  error_info &operator=(error_info &&other) noexcept(
      std::is_nothrow_move_assignable<T>::value)
  {
    value_ = std::move_if_noexcept(other.value_);
    return *this;
  }

  value_type const &value() const { return value_; }

  type_info const &type() const noexcept { return typeid_<error_info>(); }

  std::string to_string() const override
  {
    return detail::to_string_impl(*this);
  }
};

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
  friend GPCL_DECL std::string diagnostic_information(exception const &exc);

  error_info_base *error_infos_ = nullptr;

  void release() noexcept
  {
    while (error_infos_)
    {
      auto tmp = error_infos_->next_;
      delete error_infos_;
      error_infos_ = tmp;
    }
  }

public:
  exception() noexcept {}

  exception(const exception &) = delete;
  exception &operator=(const exception &) = delete;

  // exception(exception &other) : exception(std::move(other)) {}

  exception(exception &&other) noexcept : error_infos_(other.error_infos_)
  {
    other.error_infos_ = nullptr;
  }

  exception &operator=(exception &&other) noexcept
  {
    release();
    error_infos_ = other.error_infos_;
    other.error_infos_ = nullptr;
    return *this;
  }

  virtual ~exception() { release(); }

  template <typename E, typename Tag, typename T>
  friend E &&operator<<(E &&e, error_info<Tag, T> &&info)
  {
    error_info_base *copy = new error_info<Tag, T>(std::move(info));
    copy->next_ = e.error_infos_;
    e.error_infos_ = copy;
    return std::forward<E>(e);
  }

  template <typename ErrorInfo>
  friend typename ErrorInfo::value_type const *
  get_error_info(exception const &exc) noexcept
  {
    for (error_info_base *ei = exc.error_infos_; ei != nullptr; ei = ei->next_)
    {
      if (ei->type() == typeid_<ErrorInfo>())
        return std::addressof(static_cast<ErrorInfo const *>(ei)->value());
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

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &out, const exception &exc)
{
  return out << diagnostic_information(exc);
}

} // namespace gpcl

#endif // GPCL_EXCEPTION_HPP
