//
// get_mutex_for_address.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_GET_MUTEX_FOR_ADDRESS_IPP
#define GPCL_DETAIL_IMPL_GET_MUTEX_FOR_ADDRESS_IPP

#include <gpcl/detail/get_mutex_for_address.hpp>
#include <gpcl/pool_allocator.hpp>
#include <gpcl/unique_lock.hpp>
#include <gpcl/weak_ptr.hpp>

#include <unordered_map>

namespace gpcl::detail {

#if GPCL_DETAIL_MUTEX_FOR_ADDRESS_USE_SHARED_PTR

class mt_map
{
  recursive_mutex mutex_;
  std::unordered_map<const void *, weak_ptr<mutex>> map_;

public:
  shared_ptr<std::unordered_map<const void *, weak_ptr<mutex>>> lock()
  {
    unique_lock<recursive_mutex> lk(mutex_);
    shared_ptr<std::unordered_map<const void *, weak_ptr<mutex>>> result(
        &map_, [this](void *) mutable { this->mutex_.unlock(); });
    lk.release();
    return result;
  }
};

struct mutex_deleter
{
  const void *key_;

  shared_ptr<mt_map> mutex_map_;
  mutex mutex_;

  explicit mutex_deleter(shared_ptr<mt_map> mutex_map, void const *s)
      : key_(s),
        mutex_map_(std::move(mutex_map)),
        mutex_()
  {
  }

  ~mutex_deleter()
  {
    GPCL_TRY
    {
      auto map = mutex_map_->lock();
      auto it = map->find(key_);
      if (it != map->cend())
      {
        if (it->second.lock().get() == &mutex_)
          map->erase(it);
      }
    }
    GPCL_CATCH(...) {}
    GPCL_CATCH_END
  }
};

shared_ptr<mutex> get_mutex_for_address(void const *key)
{
  static const shared_ptr<mt_map> mutex_map = make_shared<mt_map>();
  if (!mutex_map)
    return shared_ptr<mutex>();

  auto map = mutex_map->lock();
  auto &wp = (*map)[key];
  if (auto sp = wp.lock())
    return sp;

  auto sp_mutex_deleter = gpcl::make_shared<mutex_deleter>(mutex_map, key);
  shared_ptr<mutex> sp(sp_mutex_deleter, &sp_mutex_deleter->mutex_);

#  if defined GPCL_POSIX
  unique_lock<mutex> lock_dummy(*sp); // ensure the mutex is initialized
#  endif
  wp = sp;
  return sp;
}

#else

mutex *get_mutex_for_address(void const *key)
{
  static mutex mtx;
  static std::unordered_map<void const *, mutex> map;

  unique_lock lock(mtx);
  return &map[key];
}

#endif

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_GET_MUTEX_FOR_ADDRESS_IPP
