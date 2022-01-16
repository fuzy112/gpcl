#ifndef GPCL_DETAIL_IMPL_WIN_ONCE_FLAG_HPP
#define GPCL_DETAIL_IMPL_WIN_ONCE_FLAG_HPP

#include <gpcl/detail/win_once_flag.hpp>
#include <gpcl/error.hpp>

namespace gpcl {

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

  GPCL_TRY { callable(std::forward<Args>(args)...); }
  GPCL_CATCH(...)
  {
    status = InitOnceComplete(&flag.opaque_, INIT_ONCE_INIT_FAILED, NULL);
    if (!status)
      std::terminate();
    throw;
  }
  GPCL_CATCH_END

  status = InitOnceComplete(&flag.opaque_, 0, NULL);
  if (!status)
    std::terminate();
}

} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_ONCE_FLAG_HPP
