//
// dump_build_info.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_DUMP_BUILD_INFO_IPP
#define GPCL_IMPL_DUMP_BUILD_INFO_IPP

#include <gpcl/dump_build_info.hpp>

namespace gpcl {

void dump_build_info(std::ostream &os)
{
  os << "GPCL_MSVC=" << GPCL_MSVC << '\n'
     << "GPCL_CLANG=" << GPCL_CLANG << '\n'
     << "GPCL_GCC=" << GPCL_GCC << '\n';

#ifdef GPCL_CONFIG_WINDOWS_SEH
  os << "GPCL_CONFIG_WINDOWS_SEH\n";
#endif

#ifdef GPCL_CONFIG_NO_WINDOWS_SEH
  os << "GPCL_CONFIG_NO_WINDOWS_SEH\n";
#endif

#ifdef GPCL_CONFIG_POSIX_SIGNALS
  os << "GPCL_CONFIG_POSIX_SIGNALS\n";
#endif

#ifdef GPCL_CONFIG_NO_POSIX_SIGNALS
  os << "GPCL_CONFIG_NO_POSIX_SIGNALS\n";
#endif

#ifdef GPCL_CONFIG_NO_EXCEPTIONS
  os << "GPCL_CONFIG_NO_EXCEPTIONS\n";
#endif

#ifdef GPCL_CONFIG_NO_RTTI
  os << "GPCL_CONFIG_NO_RTTI\n";
#endif

#ifdef GPCL_DYN_LINK
  os << "GPCL_DYN_LINK\n";
#endif

#ifdef GPCL_WINDOWS
  os << "GPCL_WINDOWS\n";
#endif

#ifdef GPCL_UNIX
  os << "GPCL_UNIX\n";
#endif

#ifdef GPCL_POSIX
  os << "GPCL_POSIX\n";
#endif

#ifdef GPCL_LINUX
  os << "GPCL_LINUX\n";
#endif

#ifdef GPCL_DISABLE_AUTO_LINKING
  os << "GPCL_DISABLE_AUTO_LINKING\n";
#endif

#ifdef GPCL_BACKTRACE_HEADER
  os << "GPCL_BACKTRACE_HEADER=" << GPCL_TO_STR(GPCL_BACKTRACE_HEADER) << '\n';
#endif

#ifdef GPCL_DEBUG
  os << "GPCL_DEBUG\n";
#endif

#ifdef GPCL_BFD
  os << "GPCL_BFD\n";
#endif

#ifdef GPCL_LLVM
  os << "GPCL_LLVM\n";
#endif

#ifdef GPCL_SQLITE
  os << "GPCL_SQLITE\n";
#endif

#ifdef GPCL_DOXYGEN
  os << "GPCL_DOXYGEN\n";
#endif

  os << std::flush;
}

} // namespace gpcl

#endif
