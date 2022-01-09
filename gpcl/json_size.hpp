#ifndef GPCL_JSON_SIZE_HPP
#define GPCL_JSON_SIZE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/json_error.hpp>
#include <gpcl/tag_invoke.hpp>

#include <type_traits>

namespace gpcl {
namespace detail {

template <typename T, typename = void>
struct is_sized : std::false_type
{
};

template <typename T>
struct is_sized<T, std::void_t<decltype(std::size(std::declval<T const &>()))>>
    : std::true_type
{
};

struct json_size_fn
{
  template <typename T,
            typename = std::enable_if_t<std::is_same_v<
                tag_invoke_result_t<json_size_fn, const T &>, std::size_t>>>
  std::size_t operator()(const T &x) const
  {
    return gpcl::tag_invoke(*this, x);
  }
};

template <typename T, std::enable_if_t<is_sized<T>::value, int> = 0>
std::size_t json_size(const T &x)
{
  return std::size(x);
}

template <typename T, std::enable_if_t<!is_sized<T>::value, int> = 0>
[[noreturn]] std::size_t json_size(const T &)
{
  throw_json_error(json_errc::operation_not_supported, "json_size", true);
}

template <typename T>
std::size_t tag_invoke(json_size_fn, const T &x)
{
  return json_size(x);
}

} // namespace detail

using json_size_fn = detail::json_size_fn;

namespace {
constexpr auto &json_size = static_const<json_size_fn>;
}

} // namespace gpcl

#endif // GPCL_JSON_SIZE_HPP
