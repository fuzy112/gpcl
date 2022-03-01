//
// win_thread.ipp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_THREAD_IPP
#define GPCL_DETAIL_IMPL_WIN_THREAD_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/detail/win_thread.hpp>
#include <gpcl/unique_ptr.hpp>

#include <Windows.h>

namespace gpcl {
namespace detail {

auto __stdcall win_thread_proc(void *arg) -> unsigned
{
  [&]() noexcept {
    GPCL_ASSERT(arg != nullptr);
    auto fn = unique_ptr<win_thread::func_base>(
        reinterpret_cast<win_thread::func_base *>(arg));

    fn->run();
  }();
  _endthread();
  GPCL_UNREACHABLE("Thread should exit at the previous line.");
}

win_thread::~win_thread()
{
  GPCL_ASSERT(!joinable());

  if (joinable())
  {
    std::terminate();
  }
}

auto win_thread::join() -> void
{
  GPCL_ASSERT(joinable());

  switch (::WaitForSingleObject(thread_.get(), INFINITE))
  {
  case WAIT_OBJECT_0:
    thread_.reset();
    return;

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

auto win_thread::detach() -> void
{
  thread_.reset();
}

auto win_thread::start_thread_impl(func_base *fn) -> void
{
  thread_.reset(
      (::HANDLE)::_beginthreadex(nullptr, 0, win_thread_proc, fn, 0, nullptr));

  if (!thread_)
  {
    DWORD err = ::GetLastError();
    delete fn;
    throw_system_error(err, "_beginthreadex");
  }
}

auto win_thread::get_id() const -> win_thread_id
{
  auto value = ::GetThreadId(native_handle());
  GPCL_THROW_LAST_ERROR_IF(!value);
  return win_thread_id{value};
}

void win_thread::yield()
{
  ::SwitchToThread();
}

auto win_thread::this_thread_id() -> win_thread_id
{
  auto value = ::GetCurrentThreadId();
  GPCL_THROW_LAST_ERROR_IF(!value);
  return win_thread_id{value};
}

auto win_thread::hardware_concurrency() -> unsigned int
{
  SYSTEM_INFO info = {};
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_THREAD_IPP
