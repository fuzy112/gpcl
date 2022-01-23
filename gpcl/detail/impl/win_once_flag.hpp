#ifndef GPCL_DETAIL_IMPL_WIN_ONCE_FLAG_HPP
#define GPCL_DETAIL_IMPL_WIN_ONCE_FLAG_HPP

#include <gpcl/detail/win_once_flag.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/scope_exit.hpp>

namespace gpcl {

namespace detail {

bool win_once_flag::initialized() const
{
  BOOL status{};
  BOOL pending{};
  status =
      InitOnceBeginInitialize(&opaque_, INIT_ONCE_CHECK_ONLY, &pending, NULL);
  return status && !pending;
}

} // namespace detail

template <typename Callable, typename... Args>
void call_once(detail::win_once_flag &flag, Callable &&callable, Args &&...args)
{
  BOOL status;
  BOOL pending = FALSE;
  status = InitOnceBeginInitialize(&flag.opaque_, 0, &pending, NULL);
  if (!status)
    detail::throw_system_error("InitOnceBeginInitialize");

  if (!pending)
    return; // already initialized

  DWORD result = INIT_ONCE_INIT_FAILED;

  const scope_exit complete{[&flag, &result] {
    BOOL status = InitOnceComplete(&flag.opaque_, result, NULL);
    if (!status)
      std::terminate();
  }};
  callable(std::forward<Args>(args)...);
  result = 0;
}

} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_ONCE_FLAG_HPP
