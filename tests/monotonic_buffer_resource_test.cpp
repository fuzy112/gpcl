#include "doctest.h"

#include <gpcl/pmr/monotonic_buffer_resource.hpp>
#include <gpcl/pmr/new_delete_resource.hpp>
#include <gpcl/pmr/synchronised_pool_resource.hpp>
#include <gpcl/pmr/tlsf_resource.hpp>

#include <map>
#include <memory>

bool is_aligned(void const *ptr, std::size_t alignment)
{
  REQUIRE(ptr != 0);
  return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

class checking_resource : public gpcl::pmr::memory_resource
{
public:
  ~checking_resource() { CHECK(allocated_memory_info_.empty()); }

protected:
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    auto p = upstream_->allocate(bytes, alignment);
    REQUIRE(allocated_memory_info_
                .emplace(std::piecewise_construct, std::make_tuple(p),
                         std::make_tuple(bytes, alignment))
                .second);
    return p;
  }

  void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override
  {
    REQUIRE(allocated_memory_info_.at(p) == std::make_tuple(bytes, alignment));
    allocated_memory_info_.erase(p);
    upstream_->deallocate(p, bytes, alignment);
  }

  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return this == &other;
  }

  gpcl::pmr::memory_resource *upstream_ = gpcl::pmr::new_delete_resource();
  std::map<void *,
           std::tuple<std::size_t /* sz */, std::size_t /* alignment */>>
      allocated_memory_info_;
};

TEST_CASE("monotonic_buffer_resource")
{
  char buffer[5];
  checking_resource resource1;
  gpcl::pmr::monotonic_buffer_resource resource(buffer, sizeof buffer,
                                                &resource1);

  auto p = resource.allocate(10, 32);
  resource.deallocate(p, 10, 32);

  resource.release();
}

TEST_CASE("tlsf_resource")
{
  checking_resource resource1;
  {
    gpcl::pmr::tlsf_resource resource(&resource1);

    auto p = resource.allocate(20, 32);
    resource.deallocate(p, 20, 32);
  }
}

TEST_CASE("synchronised_pool_resource")
{
  checking_resource resource1;
  gpcl::pmr::synchronised_pool_resource resource(&resource1);

  auto p = resource.allocate(20, 32);
  resource.deallocate(p, 20, 32);
}
