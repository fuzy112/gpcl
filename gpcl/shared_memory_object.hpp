#ifndef GPCL_SHARED_MEMORY_OBJECT_HPP
#define GPCL_SHARED_MEMORY_OBJECT_HPP

#include <gpcl/detail/config.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/posix_shared_memory.hpp>
#endif

namespace gpcl {

#if defined GPCL_DOXYGEN
class shared_memory_object : noncopyable
{
public:
  posix_shared_memory(open_only_t, czstring<> name, access_mode access,
                      mode_t mode = default_mode);

  posix_shared_memory(open_or_create_t, czstring<> name, access_mode access,
                      mode_t mode = default_mode);

  posix_shared_memory(create_only_t, czstring<> name, access_mode access,
                      mode_t mode = default_mode);

  static error_code remove(czstring<> name);

  using native_handle_type = int;

  native_handle_type native_handle() const;

  std::size_t size() const;

  void truncate(std::size_t size) const;
};
#elif defined GPCL_POSIX
using shared_memory_object = detail::posix_shared_memory;
#endif
} // namespace gpcl

#endif // GPCL_SHARED_MEMORY_OBJECT_HPP
