#ifndef GPCL_DETAIL_IMPL_DEBUG_ALLOCATOR_IPP
#define GPCL_DETAIL_IMPL_DEBUG_ALLOCATOR_IPP

#include <gpcl/detail/debug_allocator.hpp>

#include <gpcl/basic_stacktrace.hpp>


namespace gpcl::detail {

void debug_allocator_double_free(void *p, std::size_t size, std::size_t count
#if !defined GPCL_NO_RTTI
                                 ,
                                 std::type_info const &type
#endif
)
{
  std::clog << "double free!\n";
  std::clog << "address " << p << ","
            << "size: " << size << ", "
            << "count: " << count << ", "
            << "total bytes: " << size * count << ","
#if !defined GPCL_NO_RTTI
            << "type: " << type.name()
#endif
            << '\n';
  std::clog << basic_stacktrace<std::allocator<stacktrace_entry>>::current()
            << std::endl;
  std::abort();
  return;
}

debug_allocator_data::debug_allocator_data() = default;

debug_allocator_data::~debug_allocator_data()
{
  if (!alloc_records_map.empty())
  {
    std::clog << "memory leak detected!\n";

    for (auto &&[address, record] : alloc_records_map)
    {
      std::clog << "address " << address << ": "
                << "size: " << record.size << ", "
                << "count: " << record.count << ", "
                << "total bytes: " << record.size * record.count << ", "
                << "type: " << record.type->name() << '\n';
      std::clog << basic_stacktrace<std::allocator<stacktrace_entry>>::current()
                << std::endl;
    }

    std::abort();
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
