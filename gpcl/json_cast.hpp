#ifndef GPCL_JSON_CAST_HPP
#define GPCL_JSON_CAST_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/json_error.hpp>
#include <gpcl/lexical_cast.hpp>
#include <gpcl/tag_invoke.hpp>

#include <type_traits>

namespace gpcl {
namespace detail {

template <typename T, typename S, typename = void>
struct is_lexical_castable : std::false_type
{
};

template <typename T, typename S>
struct is_lexical_castable<
    T, S, std::void_t<decltype(gpcl::lexical_cast<T>(std::declval<S>()))>>
    : std::true_type
{
};

template <typename T>
struct json_cast_fn
{
  template <typename S,
            typename = std::enable_if_t<std::is_same_v<
                tag_invoke_result_t<json_cast_fn<T>, const S &>, T>>>
  T operator()(const S &s) const
  {
    return gpcl::tag_invoke(*this, s);
  }
};

template <typename T, typename S,
          std::enable_if_t<is_lexical_castable<T, S>::value, int> = 0>
T json_cast(const S &s)
{
  return lexical_cast<T>(s);
}

template <typename T, typename S,
          std::enable_if_t<!is_lexical_castable<T, S>::value, int> = 0>
T json_cast(const S &)
{
  throw_json_error(json_errc::bad_json_cast, "json_cast", true);
}

template <typename T, typename S>
T tag_invoke(json_cast_fn<T>, const S &s)
{
  return json_cast<T>(s);
}

} // namespace detail

template <typename T>
using json_cast_fn = detail::json_cast_fn<T>;

template <typename T>
constexpr json_cast_fn<T> json_cast{};

} // namespace gpcl

#endif // GPCL_JSON_CAST_HPP
