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

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/win_thread.hpp>
#include <gpcl/unique_ptr.hpp>

#ifdef GPCL_USE_BOOST_SYSTEM_ERROR
#  include <boost/exception/diagnostic_information.hpp>
#endif

#include <process.h>

namespace gpcl {
namespace detail {

auto __stdcall win_thread_proc(void *arg) -> unsigned
{
  GPCL_ASSERT(arg != nullptr);
  auto fn = unique_ptr<win_thread::func_base>(
      reinterpret_cast<win_thread::func_base *>(arg));
#ifdef GPCL_USE_BOOST_SYSTEM_ERROR
  GPCL_TRY { fn->run(); }
  GPCL_CATCH(...)
  {
    std::cerr << boost::current_exception_diagnostic_information() << std::endl;
    std::terminate();
  }
  GPCL_CATCH_END
#else
  GPCL_TRY { fn->run(); }
  GPCL_CATCH(std::exception & e)
  {
    std::cerr << e.what() << '\n';
    std::terminate();
  }
  GPCL_CATCH_END
#endif
  ::ExitThread(0);
  return 0;
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
  thread_ = make_unique_resource_checked(
      (::HANDLE)::_beginthreadex(nullptr, 0, win_thread_proc, fn, 0, nullptr),
      detail::null_handle_deleter::invalid(), detail::null_handle_deleter());

  if (!thread_)
  {
    DWORD err = ::GetLastError();
    delete fn;
    throw_system_error(err, "CreateThread");
  }
}

auto win_thread::id() const -> win_thread_id
{
  auto value = ::GetThreadId(native_handle());
  if (!value)
  {
    throw_system_error("win_thread::id");
  }
  return win_thread_id{value};
}

void win_thread::yield()
{
  ::SwitchToThread();
}

auto win_thread::this_thread_id() -> win_thread_id
{
  auto value = ::GetCurrentThreadId();
  if (!value)
  {
    throw_system_error("win_thread::id");
  }
  return win_thread_id{value};
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_WIN_THREAD_IPP
