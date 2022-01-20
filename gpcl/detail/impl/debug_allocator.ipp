#ifndef GPCL_DETAIL_IMPL_DEBUG_ALLOCATOR_IPP
#define GPCL_DETAIL_IMPL_DEBUG_ALLOCATOR_IPP

#include <gpcl/detail/debug_allocator.hpp>

#include <gpcl/debugstream.hpp>

namespace gpcl::detail {

debug_allocator_data::debug_allocator_data() = default;

debug_allocator_data::~debug_allocator_data()
{
  if (!alloc_records_map.empty())
  {
    std::clog << "memory leak detected!\n";

    for (auto &&[address, record] : alloc_records_map)
    {
      gpcl::cdebug() << "address " << address << ": "
                     << "size: " << record.size << ", "
                     << "count: " << record.count << ", "
                     << "total bytes: " << record.size * record.count << ", "
                     << "type: " << record.type->name() << '\n';
    }

    std::_Exit(1);
  }
  else
  {
    // std::clog << "no memory leak!\n";
  }
}

debug_allocator_data &debug_allocator_data::instance()
{
  static debug_allocator_data instance;
  return instance;
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_DEBUG_ALLOCATOR_IPP
