#ifndef GPCL_OUTPUT_STREAMABLE_HPP
#define GPCL_OUTPUT_STREAMABLE_HPP

#include <gpcl/detail/config.hpp>

#include <iosfwd>
#include <type_traits>

namespace gpcl {

namespace detail {

template <typename Fn>
struct iomanip
{
  Fn fn;

  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os, iomanip<Fn> const &manip)
  {
    manip.fn(os);
    return os;
  }
};

} // namespace detail

template <typename Fn, decltype(std::declval<typename std::decay<Fn>::type>()(
                                    std::declval<std::ostream &>()),
                                0) = 0>
constexpr auto make_iomanip(Fn &&fn)
{
  return detail::iomanip<std::decay_t<Fn>>{std::forward<Fn>(fn)};
}

} // namespace gpcl

#endif // GPCL_OUTPUT_STREAMABLE_HPP
