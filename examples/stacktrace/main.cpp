#include <gpcl/stacktrace.hpp>
#include <iostream>
#include <thread>

// Stacktrace is implemented using PDB files on Windows, so the PDB files must
// be available at the search path. The simplest way to ensure this is to put
// the PDB files in the same directory as the executable.

// Other platforms are currently not supported.

int main()
{
#if defined GPCL_STACKTRACE // stacktrace is supported

  std::thread([] {
    std::cerr << gpcl::stacktrace::current() << std::endl;
  }).join();
#endif
}