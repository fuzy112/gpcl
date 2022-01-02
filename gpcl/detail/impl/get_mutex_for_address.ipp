#ifndef GPCL_DETAIL_IMPL_GET_MUTEX_FOR_ADDRESS_IPP
#define GPCL_DETAIL_IMPL_GET_MUTEX_FOR_ADDRESS_IPP

#include <gpcl/detail/get_mutex_for_address.hpp>
#include <gpcl/unique_lock.hpp>
#include <gpcl/weak_ptr.hpp>

#include <unordered_map>

namespace gpcl::detail {
    
shared_ptr<mutex> get_mutex_for_address(void const *key)
{
  static recursive_mutex map_mutex;
  static std::unordered_map<const void *, weak_ptr<mutex>> map;

  unique_lock<recursive_mutex> lock(map_mutex);

  auto &wp = map[key];
  if (auto sp = wp.lock())
    return sp;

  struct mutex_deleter
  {
    const void *key_;

    mutex mutex_;

    explicit mutex_deleter(void const *s) : key_(s), mutex_() {}

    ~mutex_deleter()
    {
      unique_lock<recursive_mutex> lock(map_mutex);
      auto it = map.find(key_);
      if (it != map.cend())
      {
        GPCL_ASSERT(it->second.lock().get() == &mutex_);
        map.erase(it);
      }
    }
  };
  auto sp_mutex_deleter = gpcl::make_shared<mutex_deleter>(key);
  shared_ptr<mutex> sp(sp_mutex_deleter, &sp_mutex_deleter->mutex_);

#if defined GPCL_POSIX
  unique_lock<mutex> lock_dummy(*sp); // ensure the mutex is initialized
#endif
  wp = sp;
  return sp;
}
} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_GET_MUTEX_FOR_ADDRESS_IPP
