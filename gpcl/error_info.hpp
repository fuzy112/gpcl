#ifndef GPCL_ERROR_INFO_HPP
#define GPCL_ERROR_INFO_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/excfwd.hpp>

#include <gpcl/detail/error_info_impl.hpp>
#include <gpcl/intrusive_ptr.hpp>

#include <string>

namespace gpcl {

template <typename Tag, typename T>
class error_info
{
  friend ::gpcl::exception;
  template <typename ErrorInfo>
  friend typename ErrorInfo::value_type const *
  get_error_info(exception const &exc) noexcept;

private:
  using impl_type = detail::error_info_impl<Tag, T>;

public:
  using value_type = T;
  using tag = Tag;

  using format_fn = detail::fmt_error_info_fn<Tag, T>;

  inline static constexpr format_fn &format = detail::fmt_error_info<Tag, T>;

private:
  intrusive_ptr<impl_type> impl_;

public:
  template <typename Enable = typename std::enable_if<
                std::is_default_constructible<T>::value>::type>
  error_info() : impl_(new impl_type)
  {
  }

  template <typename... Args,
            typename std::enable_if<std::is_constructible<T, Args &&...>::value,
                                    int>::type = 0>
  explicit error_info(Args &&...args)
      : impl_(new impl_type(std::forward<Args>(args)...))
  {
  }

  template <
      typename U, typename... Args,
      typename std::enable_if<
          std::is_constructible<T, std::initializer_list<U>, Args &&...>::value,
          int>::type = 0>
  explicit error_info(std::initializer_list<U> il, Args &&...args)
      : impl_(new impl_type(il, std::forward<Args>(args)...))
  {
  }

  error_info(error_info const &) noexcept = default;
  error_info(error_info &&) noexcept = default;

  error_info &operator=(const error_info &) noexcept = default;
  error_info &operator=(error_info &&) noexcept = default;

  void swap(error_info &other) noexcept { impl_.swap(other.impl_); }

  value_type const &value() const noexcept { return impl_->value(); }
};

template <typename Tag, typename T>
void swap(error_info<Tag, T> &x, error_info<Tag, T> &y) noexcept
{
  x.swap(y);
}

template <typename Tag, typename T>
std::ostream &operator<<(std::ostream &os, error_info<Tag, T> const &ei)
{
  return os << error_info<Tag, T>::format(ei.value());
}

} // namespace gpcl

#endif // GPCL_ERROR_INFO_HPP
