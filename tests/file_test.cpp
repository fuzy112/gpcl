#include "doctest.h"

#include <gpcl/file.hpp>

using namespace gpcl;

#if defined(GPCL_POSIX)
TEST_CASE("file constructor")
{
  file f(gpcl::open_only, "/dev/null", gpcl::file::readonly);

  CHECK(f.is_open());

  f.close().value();
  CHECK_FALSE(f.is_open());

  (void)f.close();
  CHECK_FALSE(f.is_open());
}

static file f1;

static file f2{-1};

TEST_CASE("file read write")
{
  czstring<> file_name = "file_test.bin";
  (void)file::unlink(file_name);
  
  file f(gpcl::create_only, file_name, gpcl::file::readwrite);
  std::string hello = "hello world";
  std::string buf;
  buf.resize(100);

  f.write_some(gpcl::buffer(hello)).value();
  f.seek(0).value();
  std::size_t n = f.read_some(gpcl::buffer(buf)).value();
  CHECK(n == hello.size());
  buf.resize(n);
  CHECK(hello == buf);

  REQUIRE(file::unlink(file_name));
}

TEST_CASE("file read write at")
{
  czstring<> file_name = "file_test2.bin";
  (void)file::unlink(file_name);

  file f(gpcl::create_only, file_name, gpcl::file::readwrite);
  std::string hello = "hello world";
  std::string buf;
  buf.resize(100);

  auto n = f.write_some_at(128, gpcl::buffer(hello)).value();
  REQUIRE(n == hello.size());
  f.seek(128).value();
  REQUIRE(128 == f.tell().value());
  n = f.read_some(gpcl::buffer(buf)).value();
  REQUIRE(n == hello.size());
  buf.resize(n);
  REQUIRE(hello == buf);
  REQUIRE(file::unlink(file_name));
}

#endif
