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
#include <gpcl/typeid.hpp>
#include <gpcl/spanstream.hpp>

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
#if !defined(GPCL_CONFIG_NO_RTTI)
  template <typename OutIt>
  static OutIt format(OutIt out, const E &)
  {
    std::string_view str = typeid_<E>().name();
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
    ospanstream ostr(buf);
    ostr << e;
    auto written_buf = ostr.span();
    return std::copy(written_buf.begin(), written_buf.end(), out);
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
  // exception_ptr's should be thrown nested.
};

template <typename OutIt, typename E>
OutIt format_error_to(OutIt out, const E &e)
{
  return error_formatter<std::remove_cv_t<E>>::format(out, e);
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_ERROR_FORMATTER_HPP
