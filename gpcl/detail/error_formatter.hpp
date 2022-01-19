//
// error_formatter.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ERROR_FORMATTER_HPP
#define GPCL_DETAIL_ERROR_FORMATTER_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/output_iterator_streambuf.hpp>

#include <exception>
#include <ostream>
#include <string_view>
#include <type_traits>

namespace gpcl {
namespace detail {

template <typename E, typename = void>
class error_formatter
{
public:
#if !defined(GPCL_NO_RTTI)
  template <typename OutIt>
  static OutIt format(OutIt out, const E &)
  {
    std::string_view str = typeid(E).name();
    return std::copy(str.begin(), str.end(), out);
  }
#else
  template <typename OutIt>
  static OutIt format(OutIt out, const E &)
  {
    std::string_view str = "unknown error";
    return std::copy(str.begin(), str.end(), out);
  }
#endif
};

template <typename E>
class error_formatter<E, std::void_t<decltype(std::declval<std::ostream &>()
                                              << std::declval<const E &>())>>
{
public:
  template <typename OutIt>
  static OutIt format(OutIt out, const E &e)
  {
    std::array<char, 128> buf;
    output_iterator_streambuf<OutIt> sb(out);
    sb.pubsetbuf(buf.data(), buf.size());
    std::ostream ostr(&sb);
    ostr << e;
    return sb.out();
  }
};

template <>
class error_formatter<std::error_code>
{
public:
  template <typename OutIt>
  static OutIt format(OutIt out, const std::error_code &e)
  {
    auto &&msg = e.message();
    return std::copy(msg.begin(), msg.end(), out);
  }
};

template <>
class error_formatter<std::exception_ptr>
{
public:
  template <typename OutIt>
  static OutIt format(OutIt out, const std::exception_ptr &eptr)
  {
#ifndef GPCL_NO_EXCEPTIONS
    GPCL_TRY
    {
      if (eptr)
        std::rethrow_exception(eptr);
      std::string_view str{"no exception"};
      return std::copy(str.begin(), str.end(), out);
    }
    GPCL_CATCH(const std::exception &e)
    {
      std::string_view str = e.what();
      return std::copy(str.begin(), str.end(), out);
    }
    GPCL_CATCH_END
    GPCL_UNREACHABLE("failed to throw exception");
#else
    (void)eptr;
    std::string_view str = "unknown exception";
    return std::copy(str.begin(), str.end(), out);
#endif
  }
};

template <typename OutIt, typename E>
OutIt format_error_to(OutIt out, const E &e)
{
  return error_formatter<std::remove_cv_t<E>>::format(out, e);
}

} // namespace detail
} // namespace gpcl

#endif