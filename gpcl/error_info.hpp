#ifndef GPCL_ERROR_INFO_HPP
#define GPCL_ERROR_INFO_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error_info.hpp>
#include <gpcl/detail/error_info_base.hpp>
#include <gpcl/swap.hpp>

#include <iosfwd>
#include <type_traits>

namespace gpcl {

template <typename Tag, typename T>
class error_info : public detail::error_info_base
{
public:
  using tag = Tag;
  using value_type = T;

  using format_fn = detail::error_info_detail::format_error_info_fn<Tag, T>;

  static constexpr format_fn format{};

private:
  value_type value_;

public:
  template <typename Enable = typename std::enable_if<
                std::is_default_constructible<T>::value>::type>
  error_info() : value_()
  {
  }

  template <typename... Args,
            typename std::enable_if<std::is_constructible<T, Args &&...>::value,
                                    int>::type = 0>
  explicit error_info(Args &&...args) : value_(std::forward<Args>(args)...)
  {
  }

  template <
      typename U, typename... Args,
      typename std::enable_if<
          std::is_constructible<T, std::initializer_list<U>, Args &&...>::value,
          int>::type = 0>
  explicit error_info(std::initializer_list<U> il, Args &&...args)
      : value_(il, std::forward<Args>(args)...)
  {
  }

public:
  error_info(const error_info &) = default;
  error_info(error_info &&) noexcept(
      std::is_nothrow_copy_constructible<T>::value) = default;

  error_info &operator=(const error_info &) = default;
  error_info &operator=(error_info &&) noexcept(
      std::is_nothrow_copy_assignable<T>::value) = default;

  void swap(error_info &other) noexcept(std::is_nothrow_swappable<T>::value)
  {
    using gpcl::swap;
    swap(other.value_, value_);
  }

public:
  value_type const &value() const { return value_; }

  type_info const &type() const noexcept override
  {
    return typeid_<error_info<Tag, T> *>();
  }

  std::ostream &format_to(std::ostream &os) const override;
};

template <typename Tag, typename T>
void swap(error_info<Tag, T> &x,
          error_info<Tag, T> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template <typename Tag, typename T>
using format_error_info_fn = typename error_info<Tag, T>::format_fn;

template <typename Tag, typename T>
constexpr const format_error_info_fn<Tag, T> &format_error_info =
    error_info<Tag, T>::format;

template <typename Tag, typename T>
std::ostream &error_info<Tag, T>::format_to(std::ostream &os) const
{
  return os << format(value());
}

} // namespace gpcl

#endif // GPCL_ERROR_INFO_HPP
