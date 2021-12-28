#include "doctest.h"

#include <gpcl/offset_ptr.hpp>
#include <gpcl/simple_segregated_storage.hpp>

TEST_CASE("sss")
{
  gpcl::simple_segregated_storage<std::size_t, gpcl::offset_ptr<void>> sss;

  char buffer[4096];
  sss.add_block(buffer, 4096, 16);

  void *x = sss.malloc(16);
  void *y = sss.malloc(16);
  void *z = sss.malloc(16);

  sss.free(x);
  sss.free(z);
  sss.free(y);
}
