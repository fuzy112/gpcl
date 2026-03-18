//
// win_tss_ptr.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_TSS_PTR_HPP
#define GPCL_DETAIL_WIN_TSS_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/source_location.hpp>

#include <processthreadsapi.h>

namespace gpcl ::detail {

template <typename T>
class win_tss_ptr
{
public:
  win_tss_ptr()
  {
    key_ = ::TlsAlloc();
    GPCL_THROW_LAST_ERROR_IF(key_ == TLS_OUT_OF_INDEXES);
  }

  ~win_tss_ptr() { ::TlsFree(key_); }

  win_tss_ptr(const win_tss_ptr &) = delete;
  win_tss_ptr &operator=(const win_tss_ptr &) = delete;

  operator T *() const
  {
    auto value = ::TlsGetValue(key_);
    if (value == 0)
    {
      DWORD err = ::GetLastError();
      if (err != ERROR_SUCCESS)
        throw_last_error(err, "TlsGetValue",
                         GPCL_SOURCE_LOCATION_CURRENT_LINE());
    }

    return reinterpret_cast<T *>(value);
  }

  void operator=(T *value)
  {
    GPCL_THROW_LAST_ERROR_IF(!::TlsSetValue(key_, value));
  }

private:
  DWORD key_;
};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_WIN_TSS_PTR_HPP
