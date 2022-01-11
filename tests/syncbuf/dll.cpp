#include "dll.h"

namespace gpcl_syncbuf_test {

gpcl::detail::mutex_for_address_ptr
get_mutex_for_address(std::streambuf const *s)
{
  return gpcl::detail::get_mutex_for_address(s);
}

} // namespace gpcl_syncbuf_test
