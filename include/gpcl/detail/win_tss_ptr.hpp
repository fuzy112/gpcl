#ifndef GPCL_DETAIL_WIN_TSS_PTR_HPP
#define GPCL_DETAIL_WIN_TSS_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/win_once_flag.hpp>

#include <processthreadsapi.h>

namespace gpcl ::detail {

class win_tss_index
{
public:
  constexpr win_tss_index() = default;
  ~win_tss_index()
  {
    GPCL_TRY { init_once(); }
    GPCL_CATCH(...) { return; }
    GPCL_CATCH_END

    if (!::TlsFree(index_))
      abort();
  }

  void init_once()
  {
    call_once(once_, [&] {
      index_ = ::TlsAlloc();
      GPCL_THROW_LAST_ERROR_IF(index_ == TLS_OUT_OF_INDEX);
    });
  }

  void *get() const
  {
    init_once();
    void *value = ::TlsGetValue(index_);
    if (value == nullptr)
    {
      DWORD err = ::GetLastError();
      if (err != ERROR_SUCCESS)
      {
        throw_system_error(err, "get", GPCL_SOURCE_LOCATION_CURRENT_LINE);
      }
    }

    return value;
  }

  void set(void *value)
  {
    init_once();
    GPCL_THROW_LAST_ERROR_IF(::TlsSetValue(index_, value) == 0);
  }

private:
  win_once_flag once_;
  DWORD index_{};
};

template <typename T>
class win_tss_ptr
{
public:
  constexpr win_tss_ptr() = default;

  win_tss_ptr(const win_tss_ptr &) = delete;
  win_tss_ptr &operator=(const win_tss_ptr &) = delete;

  operator T *() const { return index_.get(); }

  void operator=(T *value) { index_.set(value); }

private:
  static win_tss_index index_;
};

template <typename T>
win_tss_index win_tss_ptr<T>::index_;

} // namespace gpcl::detail

#endif // GPCL_DETAIL_WIN_TSS_PTR_HPP
