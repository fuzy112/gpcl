#define GPCL_DEBUG 1
#include <gpcl/dynarray.hpp>

#include <gpcl/pmr/monotonic_buffer_resource.hpp>
#include <gpcl/pmr/polymorphic_allocator.hpp>
#include "doctest.h"

GPCL_MSVC_SUPPRESS_WARNING(26800)

TEST_CASE("dynarray")
{
  {
    gpcl::dynarray<int> arr(100);
    gpcl::dynarray arr2 = arr;
    CHECK(arr2 == arr);
    gpcl::dynarray arr3 = {1, 2, 3, 4};
    arr2 = arr;
    CHECK(arr2 == arr);
    arr = arr3;
    CHECK(arr == arr3);
  }

  SUBCASE("default construction")
  {
    gpcl::dynarray<int> a;
    CHECK(a.size() == 0);
    CHECK(a.empty());
    CHECK(a.capacity() == 0);
    CHECK(a.data() == nullptr);
    static_assert(noexcept(gpcl::dynarray<int>{}), "");
  }

  SUBCASE("construct with allocator")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::pmr::polymorphic_allocator<int> alloc2(&resource2);

    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(alloc1);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr2(alloc2);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr0;

    CHECK(arr0.get_allocator() != alloc1);
    CHECK(arr0.get_allocator() != alloc2);
    CHECK(arr0.get_allocator() == gpcl::pmr::polymorphic_allocator<int>{});

    CHECK(arr0.get_allocator() != arr1.get_allocator());
    CHECK(arr1.get_allocator() != arr2.get_allocator());
    CHECK(arr1.get_allocator() == alloc1);
    CHECK(arr2.get_allocator() == alloc2);
  }

  SUBCASE("construct n elements")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(10, alloc1);

    CHECK(arr1.size() == 10);
    CHECK(arr1.capacity() == 10);
    CHECK(arr1.get_allocator() == alloc1);

    int arr2[10] = {};
    CHECK(std::memcmp(arr1.data(), arr2, sizeof(arr2)) == 0);
  }

  SUBCASE("copy construction with polymorphic allocator")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(10, alloc1);

    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr2(arr1);
    CHECK(arr1 == arr2);
    CHECK(arr1.get_allocator() != arr2.get_allocator());
    CHECK(arr1.data() != arr2.data());
  }

  SUBCASE("copy construction with default allocator")
  {
    gpcl::dynarray<int> arr1(10);
    gpcl::dynarray<int> arr2(arr1);

    CHECK(arr1 == arr2);
    CHECK(arr1.get_allocator() == arr2.get_allocator());
    CHECK(arr1.data() != arr2.data());
  }

  SUBCASE("copy construction with allocator")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(10, alloc1);

    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr2(
        arr1, arr1.get_allocator());
    CHECK(arr1 == arr2);
    CHECK(arr1.get_allocator() == arr2.get_allocator());
    CHECK(arr1.data() != arr2.data());
  }

  SUBCASE("move construction")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(10, alloc1);
    auto data = arr1.data();

    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr2(
        std::move(arr1));
    CHECK(arr1.empty());
    CHECK(arr2.get_allocator() == alloc1);
    CHECK(arr2.data() == data);
    CHECK(arr2.size() == 10);
    CHECK(arr2.capacity() == 10);
    CHECK(arr1.size() == 0);
    CHECK(arr1.capacity() == 0);
    CHECK(arr1.data() == nullptr);
  }

  SUBCASE("move construction with same allocator")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(10, alloc1);
    auto data = arr1.data();

    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr2(
        std::move(arr1), alloc1);
    CHECK(arr1.empty());
    CHECK(arr2.get_allocator() == alloc1);
    CHECK(arr2.data() == data);
    CHECK(arr2.size() == 10);
    CHECK(arr2.capacity() == 10);
    CHECK(arr1.size() == 0);
    CHECK(arr1.capacity() == 0);
    CHECK(arr1.data() == nullptr);
  }

  SUBCASE("move construction with different allocator")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::pmr::polymorphic_allocator<int> alloc2(&resource2);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(10, alloc1);
    auto data = arr1.data();

    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr2(
        std::move(arr1), &resource2);
    // CHECK(arr1.empty());
    CHECK(arr2.get_allocator() == alloc2);
    CHECK(arr2.data() != data);
    CHECK(arr2.size() == 10);
    CHECK(arr2.capacity() == 10);
    // CHECK(arr1.size() == 0);
    // CHECK(arr1.capacity() == 0);
    // CHECK(arr1.data() == nullptr);
  }

  SUBCASE("resize, push_back, pop_back")
  {
    gpcl::pmr::monotonic_buffer_resource resource1, resource2;
    gpcl::pmr::polymorphic_allocator<int> alloc1(&resource1);
    gpcl::pmr::polymorphic_allocator<int> alloc2(&resource2);
    gpcl::dynarray<int, gpcl::pmr::polymorphic_allocator<int>> arr1(10, alloc1);

    CHECK(arr1.back() == 0);
    CHECK(arr1.size() == 10);
    CHECK(arr1.capacity() == 10);
    arr1.push_back(1);
    CHECK(arr1.size() == 11);
    CHECK(arr1.capacity() == 20);
    CHECK(arr1.back() == 1);
    arr1.pop_back();
    CHECK(arr1.back() == 0);
    CHECK(arr1.size() == 10);
    CHECK(arr1.capacity() == 20);
  }
}

#include <gpcl/span.hpp>

TEST_CASE("dynarray to span")
{
  gpcl::dynarray<int> arr{1, 2, 3};
  gpcl::span<int> span(arr);
}
