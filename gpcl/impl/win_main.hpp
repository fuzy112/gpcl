#ifndef GPCL_IMPL_WIN_MAIN_HPP
#define GPCL_IMPL_WIN_MAIN_HPP

#include <gpcl/detail/config.hpp>

#if defined(_WIN32)

#  if defined(_MSC_VER)
#    pragma comment(linker, "/subsystem:windows")
#  endif

#  include <WinBase.h>
#  include <stdlib.h>

GPCL_DECL int __cdecl main(int argc, char **argv);

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpCmdLine, int nShowCmd)
{
  (void)hInstance;
  (void)hPrevInstance;
  (void)lpCmdLine;
  (void)nShowCmd;
  return main(__argc, __argv);
}

#endif

#endif // GPCL_IMPL_WIN_MAIN_HPP
