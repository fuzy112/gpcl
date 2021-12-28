#include <gpcl/memory_mapped_region.hpp>
#include <gpcl/shared_memory_object.hpp>

int main()
{
  struct remove_shm
  {
    remove_shm() { gpcl::shared_memory_object::remove("/my_shared_object"); }
    ~remove_shm() { gpcl::shared_memory_object::remove("/my_shared_object"); }
  } remove_shm_;

  gpcl::shared_memory_object memory(gpcl::create_only, "/my_shared_object",
                                    gpcl::access_mode::read_write);
  memory.truncate(1024);

  gpcl::memory_mapped_region mapped(memory, gpcl::access_mode::read_write);

  std::memset(mapped.address(), 1, mapped.size());
}
