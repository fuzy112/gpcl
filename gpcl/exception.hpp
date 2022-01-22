#ifndef GPCL_EXCEPTION_HPP
#define GPCL_EXCEPTION_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/lexical_cast.hpp>

#include <ostream>
#include <set>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace gpcl {

class error_info_base
{
public:
  virtual ~error_info_base() = default;

  virtual const std::type_info &type() const noexcept = 0;

  virtual std::string to_string() const = 0;
};

template <typename Tag, typename T>
class error_info;

template <typename Tag, typename T>
std::string to_string(error_info<Tag, T> const &errinfo);

namespace detail {
template <typename Tag, typename T>
std::string to_string_impl(error_info<Tag, T> const &errinfo)
{
  return to_string(errinfo);
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

  std::type_info const &type() const noexcept { return typeid(error_info); }

  std::string to_string() const override
  {
    return detail::to_string_impl(*this);
  }
};

template <typename Tag, typename T>
std::string to_string(error_info<Tag, T> const &errinfo)
{
  std::ostringstream oss;
  oss << "[" << typeid(Tag *).name() << "] = { "
      << lexical_cast<std::string>(errinfo.value()) << " }";
  return oss.str();
}

template <typename CharT, typename Traits, typename Tag, typename T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &out,
           error_info<Tag, T> const &errinfo)
{
  return out << to_string(errinfo);
}

class exception;

GPCL_DECL
std::string diagnostic_information(exception const &exc);

class exception
{
  friend GPCL_DECL std::string diagnostic_information(exception const &exc);

  struct error_info_compare
  {
    using is_transparent = int;

    bool operator()(const error_info_base *x,
                    const error_info_base *y) const noexcept
    {
      if (!y)
        return false;
      if (!x)
        return true;
      return x->type().before(y->type());
    }

    bool operator()(const error_info_base *x,
                    std::type_info const &y) const noexcept
    {
      if (!x)
        return true;
      return x->type().before(y);
    }

    bool operator()(const std::type_info &x,
                    const error_info_base *y) const noexcept
    {
      if (!y)
        return false;
      return x.before(y->type());
    }
  };

  std::set<error_info_base *, error_info_compare> error_infos_;

public:
  exception() noexcept {}

  exception(const exception &) = delete;
  exception &operator=(const exception &) = delete;

  // exception(exception &other) : exception(std::move(other)) {}

  exception(exception &&other) noexcept
      : error_infos_(std::move(other).error_infos_)
  {
    other.error_infos_.clear();
  }

  exception &operator=(exception &&other) noexcept
  {
    error_infos_.swap(other.error_infos_);
    return *this;
  }

  virtual ~exception()
  {
    for (auto *p : error_infos_)
    {
      delete p;
    }
  }

  template <typename E, typename Tag, typename T>
  friend E &&operator<<(E &&e, error_info<Tag, T> &&info)
  {
    error_info_base *copy = new error_info<Tag, T>(std::move(info));
    e.error_infos_.insert(copy);
    return std::forward<E>(e);
  }

  template <typename ErrorInfo>
  friend ErrorInfo const *get_error_info(exception const &exc) noexcept
  {
    auto iter = error_infos_.find(typeid(ErrorInfo));
    if (iter != error_infos_.cend())
    {
      return *iter;
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
