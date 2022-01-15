#include <gpcl/function.hpp>
#include <gpcl/stacktrace.hpp>
#include <gpcl/thread.hpp>
#include <iostream>

// Stacktrace is implemented using PDB files on Windows, so the PDB files must
// be available at the search path. The simplest way to ensure this is to put
// the PDB files in the same directory as the executable.

// On Linux and some Unix systems, stacktrace is implemented in terms of GNU
// extension function backtrace(), which requires the program to be linked with
// -rdynamic command line option.

// Other platforms are currently not supported.

int main()
{
#if defined GPCL_STACKTRACE // stacktrace is supported

  gpcl::thread(gpcl::function<void()>([] {
    std::cerr << gpcl::stacktrace::current() << std::endl;
  })).join();
#endif
}