#include <gpcl/pmr/polymorphic_allocator.hpp>
#include <gpcl/pmr/synchronised_pool_resource.hpp>

#include <vector>

int main() {
  gpcl::pmr::synchronised_pool_resource resource;
  gpcl::pmr::polymorphic_allocator<int> alloc(&resource);

  std::vector<int, decltype(alloc)> vec(alloc);

  for (int i = 0; i != 1000; ++i)
  {
    vec.push_back(i);
  }
}