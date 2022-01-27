#ifndef GPCL_DETAIL_ERROR_INFO_IMPL_HPP
#define GPCL_DETAIL_ERROR_INFO_IMPL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error_info_base.hpp>

#include <gpcl/make_iomanip.hpp>
#include <gpcl/tag_invoke.hpp>

namespace gpcl::detail {

namespace error_info_detail {

template <typename Tag, typename T>
struct format_error_info_fn
{
  template <
      typename U = T,
      typename Result = tag_invoke_result_t<format_error_info_fn<Tag, U>, U>,
      typename Enable = decltype((std::declval<std::ostream &>()
                                  << std::declval<Result>()),
                                 void())>
  Result operator()(const T &value) const
  {
    return gpcl::tag_invoke(*this, value);
  }
};

template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const T &)
{
  using std::operator<<;
  return os << "{unknown value}";
}

template <typename Tag, typename T>
auto tag_invoke(format_error_info_fn<Tag, T>, const T &v)
{
  return gpcl::make_iomanip([&](auto &s) {
    s << '[' << typeid_<Tag *>().name() << "] = {" << v << '}';
  });
}

} // namespace error_info_detail

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ERROR_INFO_IMPL_HPP
