#include <gpcl/buffer.hpp>
#include <gpcl/buffers_range_ref.hpp>

#include "doctest.h"
#include <iostream>
#include <array>

struct pod
{
    char a;
    unsigned long b;
    unsigned c;
};

TEST_CASE("buffers_range_ref") {
  pod data;
  
  std::array<gpcl::const_buffer, 3> buffer_sequence{
      gpcl::buffer(&data.a, 1),
      gpcl::buffer(&data.b, sizeof(data.b)),
      gpcl::buffer(&data.c, sizeof(data.c)),
  };

  std::size_t total_size = 0;
  auto ref = gpcl::buffers_range_ref(buffer_sequence);
  for (auto b : ref) {
      total_size += b.size();
  }

  CHECK(total_size == sizeof(data.a) + sizeof(data.b) + sizeof(data.c));
}