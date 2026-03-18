//
// excfwd.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EXCFWD_HPP
#define GPCL_EXCFWD_HPP

#include <gpcl/detail/config.hpp>

#include <iosfwd>
#include <tuple>
#include <type_traits>
#include <string>

namespace gpcl {

class exception;

template <typename Tag, typename T>
class error_info;

template <typename Tag, typename T>
std::string to_string(const error_info<Tag, T> &err_info);

template <typename CharT, typename Traits, typename Tag, typename T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           error_info<Tag, T> const &err_info);

template <typename CharT, typename Traits, typename E,
          typename std::enable_if<std::is_base_of<exception, E>::value,
                                  int>::type = 0>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &out, const E &exc);

template <typename T>
struct is_error_info : std::false_type
{
};

template <typename Tag, typename T>
struct is_error_info<error_info<Tag, T>> : std::true_type
{
};

template <typename E, typename ErrorInfo,
          typename std::enable_if<
              std::is_base_of<exception, typename std::decay<E>::type>::value &&
                  is_error_info<typename std::decay<ErrorInfo>::type>::value,
              int>::type = 0>
auto operator<<(const E &e, ErrorInfo &&err_info) noexcept;

template <
    typename E, typename... ErrorInfos,
    typename std::enable_if<
        std::conjunction<
            std::is_base_of<exception, typename std::decay<E>::type>,
            is_error_info<typename std::decay<ErrorInfos>::type>...>::value,
        int>::type = 0>
auto operator<<(const E &e,
                const std::tuple<ErrorInfos...> &error_infos) noexcept;

template <typename E>
auto diagnostic_information(const E &e);

template <typename ErrorInfo>
typename ErrorInfo::value_type const *
get_error_info(exception const &exc) noexcept;

} // namespace gpcl

#endif // GPCL_EXCFWD_HPP
