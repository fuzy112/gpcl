//
// object_pool.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OBJECT_POOL_HPP
#define GPCL_OBJECT_POOL_HPP

#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/pool.hpp>
#include <gpcl/simple_segregated_storage.hpp>

namespace gpcl {

template <typename T, typename UA = default_new_delete_user_allocator,
          typename MutexType = gpcl::mutex>
class object_pool
{
  struct chunk_info
  {
    T elem;
    chunk_info *prev;
    chunk_info *next;
  };

public:
  using element_type = T;
  using user_allocator = UA;
  using mutex_type = MutexType;
  using size_type = typename user_allocator::size_type;
  using difference_type = typename user_allocator::difference_type;

  object_pool()
      : p_(detail::piecewise_construct, std::make_tuple(sizeof(chunk_info)),
           std::make_tuple()),
        obj_list_()
  {
  }

  ~object_pool()
  {
    while (obj_list_)
    {
      obj_list_->elem.~element_type();
      obj_list_ = obj_list_->next;
    }
  }

  element_type *malloc()
  {
    gpcl::unique_lock<mutex_type> lock(get_mutex());

    auto *obj = reinterpret_cast<chunk_info *>(get_pool().malloc());
    obj->prev = nullptr;
    obj->next = obj_list_;
    if (obj_list_)
      obj_list_->prev = obj;
    obj_list_ = obj;
    return &obj->elem;
  }

  void free(element_type *p)
  {
    GPCL_ASSERT(p != nullptr);
    auto *obj = reinterpret_cast<chunk_info *>(p);
    gpcl::unique_lock<mutex_type> lock(get_mutex());

    auto prev = obj->prev;
    auto next = obj->next;
    if (prev)
      prev->next = next;
    if (next)
      next->prev = prev;
    get_pool().free(p);
    if (obj_list_ == obj)
      obj_list_ = next;
  }

  bool is_from(element_type *p) { return p_.first().is_from(p); }

  element_type *construct() { return new (this->malloc()) element_type(); }

  template <typename... Args>
  element_type *construct(Args &&... args)
  {
    return new (this->malloc()) element_type(std::forward<Args>(args)...);
  }

  void destroy(element_type *p)
  {
    p->~element_type();
    this->free(p);
  }

  size_type get_next_size() const { p_.first().get_next_size(); }

  void set_next_size(size_type next_size)
  {
    p_.first().set_next_size(next_size);
  }

private:
  pool<user_allocator, null_mutex> &get_pool() { return p_.first(); }

  mutex_type &get_mutex() { return p_.second(); }

  detail::compressed_pair<pool<user_allocator, null_mutex>, mutex_type> p_;
  chunk_info *obj_list_;
};

} // namespace gpcl
#endif // GPCL_OBJECT_POOL_HPP
