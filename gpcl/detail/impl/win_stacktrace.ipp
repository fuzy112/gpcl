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

#include <gpcl/detail/win_stacktrace.hpp>

namespace gpcl::detail {

win_dbg_helper::win_dbg_helper()
{
  auto lk = lock();

  SymInitialize(process(), NULL, TRUE);
  SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
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

  auto lock = g_win_dbg_helper.lock();
  if (SymFromAddr(g_win_dbg_helper.process(), data_.AddrPC.Offset,
                  &displacement, symbol))
  {
    return std::string(symbol->Name, symbol->NameLen);
  }

  return "";
}

std::uint_least32_t win_stacktrace_entry::source_line() const
{
  DWORD64 dwAddress = data_.AddrPC.Offset;
  DWORD dwDisplacement;
  IMAGEHLP_LINE64 line;

  auto lock = g_win_dbg_helper.lock();
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

  if (SymGetLineFromAddr64(g_win_dbg_helper.process(), dwAddress,
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
  DWORD64 dwAddress = data_.AddrPC.Offset;
  DWORD dwDisplacement;
  IMAGEHLP_LINE64 line;

  auto lock = g_win_dbg_helper.lock();
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

  if (SymGetLineFromAddr64(g_win_dbg_helper.process(), dwAddress,
                           &dwDisplacement, &line))
  {
    return line.FileName;
  }

  IMAGEHLP_MODULE64 module_;
  module_.SizeOfStruct = sizeof(module_);
  if (SymGetModuleInfo64(g_win_dbg_helper.process(), dwAddress, &module_))
  {
    return module_.LoadedImageName;
  }

  return "unknown";
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_WIN_STACKTRACE_IPP
