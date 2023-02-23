#include <gpcl/out_ptr.hpp>

#include <gpcl/shared_ptr.hpp>
#include <gpcl/unique_ptr.hpp>


#include "doctest.h"

namespace {

void f1(void **p)
{
  *p = new int(32);
}

void f2(int **p)
{
  *p = new int(64);
}
} // namespace

TEST_CASE("out_ptr")
{
  gpcl::unique_ptr<int> p;
  f1(gpcl::out_ptr(p));
  CHECK(*p == 32);

  f2(gpcl::out_ptr(p));
  CHECK(*p == 64);

  int *rp = nullptr;
  f1(gpcl::out_ptr(rp));
  CHECK(*rp == 32);
  delete rp;
  f2(gpcl::out_ptr(rp));
  CHECK(*rp == 64);
  delete rp;

  gpcl::shared_ptr<int> sp;
  f1(gpcl::out_ptr(sp));
  CHECK(*sp == 32);
  f1(gpcl::out_ptr(sp, gpcl::default_delete<int>()));
  CHECK(*sp == 32);
  f2(gpcl::out_ptr(sp));
  CHECK(*sp == 64);
  f2(gpcl::out_ptr(sp, gpcl::default_delete<int>()));
  CHECK(*sp == 64);
}


#ifdef GPCL_WINDOWS
#include <gpcl/detail/win_module_handle.hpp>

TEST_CASE("out_ptr<unique_handle>")
{
    gpcl::detail::module_handle hmodule;
    REQUIRE(::GetModuleHandleExA(0, NULL, gpcl::out_ptr<HMODULE>(hmodule)));
}
#endif