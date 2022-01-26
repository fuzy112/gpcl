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
#include <gpcl/make_iomanip.hpp>

namespace gpcl {

class exception
{
  template <typename E>
  friend auto diagnostic_information(E const &exc);

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

  exception(const exception &) = default;
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
  void add_error_info(const error_info<Tag, T> &info) noexcept
  {
    auto &ei = *info.impl_;
    auto impl_ptr = info.impl_;
    impl_ptr.release();
    error_info_list_.push_back(ei);
  }

  template <typename Tag, typename T>
  void add_error_info(error_info<Tag, T> &&info) noexcept
  {
    auto &ei = *info.impl_;
    info.impl_.release();
    error_info_list_.push_back(ei);
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

template <
    typename CharT, typename Traits, typename E,
    typename std::enable_if<std::is_base_of<exception, E>::value, int>::type>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &out, const E &exc)
{
  return out << diagnostic_information(exc);
}

template <typename E, typename ErrorInfo,
          typename std::enable_if<
              std::is_base_of<exception, typename std::decay<E>::type>::value &&
                  is_error_info<typename std::decay<ErrorInfo>::type>::value,
              int>::type>
E &&operator<<(E &&e, ErrorInfo &&err_info) noexcept
{
  e.add_error_info(std::forward<ErrorInfo>(err_info));
  return std::forward<E>(e);
}

template <
    typename E, typename... ErrorInfos,
    typename std::enable_if<
        std::conjunction<
            std::is_base_of<exception, typename std::decay<E>::type>,
            is_error_info<typename std::decay<ErrorInfos>::type>...>::value,
        int>::type>
E &&operator<<(E &&e, const std::tuple<ErrorInfos...> &error_infos) noexcept
{
  std::apply([&e](auto &&...err_infos) { [](...) {}(&(e << err_infos)...); });
  return std::forward<E>(e);
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
auto diagnostic_information(const E &e)
{
  return make_iomanip([&](std::ostream &s) {
    s << "Exception [" << exception_name(e) << "]\n";

    gpcl::exception const *ge = dyn_cast<gpcl::exception>(&e);
    if (!ge)
      return;

    for (const detail::error_info_base &ei : ge->error_info_list_)
    {
      s << "  ";
      ei.format_to(s) << "\n";
    }
  });
}

} // namespace gpcl

#endif // GPCL_EXCEPTION_HPP
