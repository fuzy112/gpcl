#ifndef GPCL_DETAIL_ERROR_INFO_IMPL_HPP
#define GPCL_DETAIL_ERROR_INFO_IMPL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error_info_base.hpp>

#include <gpcl/make_iomanip.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/tag_invoke.hpp>

namespace gpcl::detail {

template <typename Tag, typename T>
class error_info_impl : public error_info_base, private noncopyable
{
  friend ::gpcl::error_info<Tag, T>;

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

  template <typename... Args,
            typename std::enable_if<std::is_constructible<T, Args &&...>::value,
                                    int>::type = 0>
  explicit error_info_impl(Args &&...args) : value_(std::forward<Args>(args)...)
  {
  }

  template <
      typename U, typename... Args,
      typename std::enable_if<
          std::is_constructible<T, std::initializer_list<U>, Args &&...>::value,
          int>::type = 0>
  explicit error_info_impl(std::initializer_list<U> il, Args &&...args)
      : value_(il, std::forward<Args>(args)...)
  {
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
struct fmt_error_info_fn
{
  template <typename U = T,
            typename Result = tag_invoke_result_t<fmt_error_info_fn<Tag, U>, U>,
            typename Enable = decltype((std::declval<std::ostream &>()
                                        << std::declval<Result>()),
                                       void())>
  Result operator()(const T &value) const
  {
    return gpcl::tag_invoke(*this, value);
  }
};

template <typename Tag, typename T>
constexpr fmt_error_info_fn<Tag, T> fmt_error_info{};

namespace error_info_detail {
template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const T &)
{
  using std::operator<<;
  return os << "{unknown value}";
}
} // namespace error_info_detail

template <typename Tag, typename T>
auto tag_invoke(fmt_error_info_fn<Tag, T>, const T &v)
{
  return gpcl::make_iomanip([&](auto &s) {
    using error_info_detail::operator<<;
    s << '[' << typeid_<Tag *>().name() << "] = {" << v << '}';
  });
}

template <typename Tag, typename T>
std::ostream &error_info_impl<Tag, T>::format_to(std::ostream &os) const
{
  return os << fmt_error_info<Tag, T>(value());
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ERROR_INFO_IMPL_HPP
