#ifndef GPCL_EXCFWD_HPP
#define GPCL_EXCFWD_HPP

#include <gpcl/detail/config.hpp>

#include <iosfwd>
#include <type_traits>

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

template <
    typename E, typename Tag, typename T,
    typename std::enable_if<
        std::is_base_of<typename std::decay<E>::type, E>::value, int>::type = 0>
E &&operator<<(const E &e, const error_info<Tag, T> &err_info) noexcept;

template <typename E>
std::string diagnostic_information(const E &e);

} // namespace gpcl

#endif // GPCL_EXCFWD_HPP
