#ifndef GPCL_DETAIL_WIN_ONCE_FLAG_HPP
#define GPCL_DETAIL_WIN_ONCE_FLAG_HPP

#include <gpcl/detail/config.hpp>

#include <windows.h>

namespace gpcl {
namespace detail {
class win_once_flag;
}

template <typename Callable, typename... Args>
void call_once(detail::win_once_flag &flag, Callable &&callable,
               Args &&...args);

namespace detail {

class win_once_flag
{
  INIT_ONCE opaque_ = INIT_ONCE_STATIC_INIT;

public:
  constexpr win_once_flag() = default;
  win_once_flag(const win_once_flag &) = delete;
  win_once_flag &operator=(const win_once_flag) = delete;

  template <typename Callable, typename... Args>
  friend void ::gpcl::call_once(detail::win_once_flag &flag,
                                Callable &&callable, Args &&...args);
};

} // namespace detail
} // namespace gpcl

#include <gpcl/detail/impl/win_once_flag.hpp>

#endif // GPCL_DETAIL_WIN_ONCE_FLAG_HPP
