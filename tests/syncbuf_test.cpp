#include "doctest.h"
#include "syncbuf/dll.h"

TEST_CASE("syncbuf mutex map test")
{
  std::streambuf *p(0);
  auto pm1 = gpcl::detail::get_mutex_for_address(p);
  auto pm2 = gpcl_syncbuf_test::get_mutex_for_address(p);

  REQUIRE(pm1 == pm2);
}
