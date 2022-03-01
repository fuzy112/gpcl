//
// win_stacktrace.ipp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_WIN_STACKTRACE_IPP
#define GPCL_DETAIL_IMPL_WIN_STACKTRACE_IPP

#include <gpcl/detail/win_mutex.hpp>
#include <gpcl/detail/win_stacktrace.hpp>
#include <gpcl/detail/throw_system_error.hpp>

#if defined(GPCL_NO_STACKTRACE)
#  error "This header should no be included."
#endif

#if !defined(__CYGWIN__) && !defined(GPCL_BFD)
#  include <DbgHelp.h>
#else
#  include <gpcl/detail/bfd_stacktrace.hpp>
#endif

namespace gpcl::detail {

#if !defined(__CYGWIN__) && !defined(GPCL_BFD)
win_dbg_helper::win_dbg_helper()
{
  auto lk = lock();

  if (!SymInitialize(process(), NULL, TRUE))
  {
    throw_system_error(__func__);
  }
  SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
}

win_dbg_helper::~win_dbg_helper()
{
  auto lk = lock();
  SymCleanup(process());
}

win_dbg_helper &win_dbg_helper::instance()
{
  static win_dbg_helper inst;
  return inst;
}

unique_lock<win_recursive_mutex> win_dbg_helper::lock()
{
  static win_recursive_mutex win_dbg_mtx{};
  return unique_lock(win_dbg_mtx);
}

std::string win_stacktrace_entry::description() const
{
  char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + sizeof(char)];
  PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbol->MaxNameLen = MAX_SYM_NAME;
  DWORD64 displacement = 0;

  BOOL status;

  auto lock = g_win_dbg_helper.lock();
  status = SymFromAddr(g_win_dbg_helper.process(), (std::intptr_t)addr_,
                       &displacement, symbol);

  if (status)
    return std::string(symbol->Name, symbol->NameLen);
  return "";
}

std::uint_least32_t win_stacktrace_entry::source_line() const
{
  DWORD dwDisplacement;
  IMAGEHLP_LINE64 line;

  auto lock = g_win_dbg_helper.lock();
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

  if (SymGetLineFromAddr64(g_win_dbg_helper.process(), (std::intptr_t)addr_,
                           &dwDisplacement, &line))
  {
    return line.LineNumber;
  }
  else
  {
    return 0;
  }
}

std::string win_stacktrace_entry::source_file() const
{
  DWORD dwDisplacement;
  IMAGEHLP_LINE64 line;

  auto lock = g_win_dbg_helper.lock();
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

  if (SymGetLineFromAddr64(g_win_dbg_helper.process(), (std::intptr_t)addr_,
                           &dwDisplacement, &line))
  {
    return line.FileName;
  }

  return "";
}

std::string win_stacktrace_entry::binary_file() const
{
  IMAGEHLP_MODULE64 module64;
  module64.SizeOfStruct = sizeof(module64);
  if (SymGetModuleInfo64(g_win_dbg_helper.process(), (std::intptr_t)addr_,
                         &module64))
  {
    return module64.LoadedImageName;
  }
  return "";
}

#else

std::string win_stacktrace_entry::description() const
{
  return bfd_stacktrace_entry_description(addr_);
}

std::string win_stacktrace_entry::source_file() const
{
  return bfd_stacktrace_entry_source_file(addr_);
}

std::uint_least32_t win_stacktrace_entry::source_line() const
{
  return bfd_stacktrace_entry_source_line(addr_);
}

#endif

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_WIN_STACKTRACE_IPP
