#include <gpcl/buffer.hpp>
#include <gpcl/buffers_range_ref.hpp>
#include <gpcl/flat_buffer.hpp>

#include "doctest.h"
#include <array>
#include <iostream>

struct pod
{
  char a;
  unsigned long b;
  unsigned c;
};

TEST_CASE("buffers_range_ref")
{
  pod data;

  std::array<gpcl::const_buffer, 3> buffer_sequence{
      gpcl::buffer(&data.a, 1),
      gpcl::buffer(&data.b, sizeof(data.b)),
      gpcl::buffer(&data.c, sizeof(data.c)),
  };

  std::size_t total_size = 0;
  auto ref = gpcl::buffers_range_ref(buffer_sequence);
  for (auto b : ref)
  {
    total_size += b.size();
  }

  CHECK(total_size == sizeof(data.a) + sizeof(data.b) + sizeof(data.c));
}


TEST_CASE("flat_buffer")
{
  gpcl::flat_buffer fb;

  for (int i = 0; i < 100; ++i)
  {
    auto b = fb.prepare(11);
    REQUIRE(b.size() >= 11);
    std::memcpy(b.data(), "Hello world", 11);
    fb.commit(6);
    CHECK(fb.size() == 6);
    auto b1 = fb.data();
    CHECK(b1.size() == 6);
    CHECK(std::string("Hello ") == std::string((const char *)b1.data(), 6));
    fb.consume(6);
    CHECK(fb.size() == 0);
  }
}

