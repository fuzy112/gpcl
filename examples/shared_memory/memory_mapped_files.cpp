#include <gpcl/file.hpp>
#include <gpcl/memory_mapped_region.hpp>

int main()
{
  gpcl::file afile(gpcl::open_or_create, "afile.bin",
                   gpcl::file::access_mode::readwrite);
  afile.truncate(gpcl::memory_mapped_region::page_size() * 10).value();

  gpcl::memory_mapped_region mapped(afile, gpcl::access_mode::read_write);

  std::memset(mapped.address(), 0xff, mapped.size());
}
