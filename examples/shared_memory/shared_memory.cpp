#include <gpcl/memory_mapped_region.hpp>
#include <gpcl/shared_memory_object.hpp>
#include <gpcl/debugstream.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/anonymous_shared_memory.hpp>

int main()
{
	GPCL_TRY {
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

  gpcl::memory_mapped_region mapped2(
      gpcl::anonymous_shared_memory(1024), gpcl::read_write);

  std::memset(mapped2.address(), 1, mapped2.size());
 } GPCL_CATCH (const std::exception &exc) {
	 gpcl::cdebug() << gpcl::diagnostic_information(exc) << std::endl;
 } GPCL_CATCH_END
}
