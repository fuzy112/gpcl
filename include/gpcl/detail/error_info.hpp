//
// error_info.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ERROR_INFO_IMPL_HPP
#define GPCL_DETAIL_ERROR_INFO_IMPL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error_info_base.hpp>

#include <gpcl/make_iomanip.hpp>
#include <gpcl/tag_invoke.hpp>

namespace gpcl {
namespace detail {

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

class error_info_ref
{
  const error_info_base *p_ = nullptr;

public:
  error_info_ref() noexcept = default;

  explicit error_info_ref(const error_info_base &e) noexcept : p_(&e) {}
  explicit error_info_ref(const error_info_base &&) = delete;

  bool next() noexcept { return (p_ = p_->next_); }

  bool is_valid() const noexcept { return p_ != nullptr; }

  const type_info &type() const noexcept { return p_->type(); }

  std::ostream &format_to(std::ostream &os) const { return p_->format_to(os); }

  bool operator==(error_info_ref const &other) const noexcept
  {
    return p_ == other.p_;
  }

  const error_info_base *get() const noexcept { return p_; }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_ERROR_INFO_IMPL_HPP
