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
#include <gpcl/generic_pointer_cast.hpp>
#include <gpcl/make_iomanip.hpp>

namespace gpcl {

class exception
{
  template <
      typename E, typename ErrorInfo,
      typename std::enable_if<
          std::is_base_of<exception, typename std::decay<E>::type>::value &&
              is_error_info<typename std::decay<ErrorInfo>::type>::value,
          int>::type>
  friend auto operator<<(const E &e, ErrorInfo &&err_info) noexcept;

  template <typename E>
  friend auto diagnostic_information(E const &exc);

  template <typename ErrorInfo>
  friend typename ErrorInfo::value_type const *
  get_error_info(const exception &e) noexcept;

protected:
  detail::error_info_base const *list_ = nullptr;

public:
  exception() noexcept = default;

  exception(const exception &) = default;
  exception &operator=(const exception &) = default;

  exception(exception &&other) noexcept = default;

  exception &operator=(exception &&other) noexcept = default;

  virtual ~exception() = default;

protected:
  static const detail::error_info_base *&
  next(const detail::error_info_base *e) noexcept
  {
    GPCL_ASSERT(e);
    return e->next_;
  }

  void update_error_infos() { list_ = nullptr; }
};

namespace detail {

template <typename Base, typename... ErrorInfos>
class exception_with_error_info : public Base
{
  std::tuple<ErrorInfos...> error_infos_;

protected:
  void update_error_infos()
  {
    // Base::update_error_infos();

    const error_info_base **pp = &this->list_;
    while (*pp)
      pp = &this->next(*pp);
    *pp = link_error_infos(error_infos_);
  }

public:
  exception_with_error_info(const Base &base,
                            const std::tuple<ErrorInfos...> &error_infos)
      : Base(base),
        error_infos_(error_infos)
  {
    update_error_infos();
  }

  exception_with_error_info(const exception_with_error_info &other)
      : Base(static_cast<const Base &>(other)),
        error_infos_(other.error_infos_)
  {
    update_error_infos();
  }

  exception_with_error_info &operator=(const exception_with_error_info &other)
  {
    static_cast<Base &>(*this) = static_cast<const Base &>(other);
    error_infos_ = other.error_infos_;
    update_error_infos();
    return *this;
  }
};

} // namespace detail

template <typename ErrorInfo>
typename ErrorInfo::value_type const *
get_error_info(exception const &exc) noexcept
{
  for (const detail::error_info_base *p = exc.list_; p != nullptr;
       p = exc.next(p))
  {
    if (p->type() == typeid_<ErrorInfo *>())
      return std::addressof(static_pointer_cast<const ErrorInfo>(p)->value());
  }
  return nullptr;
}

template <typename E, typename ErrorInfo,
          typename std::enable_if<
              std::is_base_of<exception, typename std::decay<E>::type>::value &&
                  is_error_info<typename std::decay<ErrorInfo>::type>::value,
              int>::type>
auto operator<<(const E &e, ErrorInfo &&err_info) noexcept
{
  return detail::exception_with_error_info<
      E, typename std::decay<ErrorInfo>::type>(e, std::make_tuple(err_info));
}

template <
    typename CharT, typename Traits, typename E,
    typename std::enable_if<std::is_base_of<exception, E>::value, int>::type>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &out, const E &exc)
{
  return out << diagnostic_information(exc);
}

template <
    typename E, typename... ErrorInfos,
    typename std::enable_if<
        std::conjunction<
            std::is_base_of<exception, typename std::decay<E>::type>,
            is_error_info<typename std::decay<ErrorInfos>::type>...>::value,
        int>::type>
auto operator<<(const E &e,
                const std::tuple<ErrorInfos...> &error_infos) noexcept
{
  return detail::exception_with_error_info<E, ErrorInfos...>(e, error_infos);
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

    for (const detail::error_info_base *p = ge->list_; p != nullptr;
         p = ge->next(p))
    {
      auto &ei = *p;
      s << "  ";
      ei.format_to(s) << "\n";
    }
  });
}

} // namespace gpcl

#endif // GPCL_EXCEPTION_HPP
