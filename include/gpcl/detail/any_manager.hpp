//
// any_manager.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ANY_MANAGER_HPP
#define GPCL_DETAIL_ANY_MANAGER_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/detail/assert.hpp>
#include <gpcl/typeid.hpp>

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>


namespace gpcl {
namespace detail {

/// An type that can store data of any type in it.
/// @tparam LocalSize the maximum size of type stored locally.
/// @tparam LocalAlign the maximum alignment of type stored locally.
template <std::size_t LocalSize, std::size_t LocalAlign>
union any_data
{
  static_assert(LocalSize >= sizeof(void *));

  // local storage buffer.
  std::aligned_storage_t<LocalSize, LocalAlign> local_buffer;

  // pointer to remote storage.
  void *remote_addr;
};

enum class any_manage_op : char
{
  get_pointer = 1,
  get_type_info = 2,
  clone = 3,
  move = 4,
  destroy = 5,
};

template <std::size_t LocalSize, std::size_t LocalAlign>
using any_manage_func_t = void *(*)(any_data<LocalSize, LocalAlign> *,
                                    const any_data<LocalSize, LocalAlign> *,
                                    any_manage_op);

template <typename T, std::size_t LocalSize, std::size_t LocalAlign>
struct any_manager
{
  using any_data_t = any_data<LocalSize, LocalAlign>;

  // determines whether T should be stored locally.
  static constexpr std::integral_constant<
      bool, sizeof(T) <= LocalSize && alignof(T) <= LocalAlign &&
                LocalAlign % alignof(T) == 0 &&
                std::is_nothrow_move_constructible<T>::value>
      local_storage{};

  // get pointer to the locally stored data.
  static T *get_pointer(const any_data_t *source, std::true_type) noexcept
  {
    return const_cast<T *>(
        std::launder(reinterpret_cast<const T *>(&source->local_buffer)));
  }

  // get pointer to the remotely stored data.
  static T *get_pointer(const any_data_t *source, std::false_type) noexcept
  {
    return static_cast<T *>(source->remote_addr);
  }

  // get the type_info of T.
  static const type_info *get_type_info(const any_data_t *source) noexcept
  {
    (void)source;
    return &typeid_<T>();
  }

  // clone the data.
  static void clone(any_data_t *dest, const any_data_t *source, std::true_type)
  {
    ::new (&dest->local_buffer) T(*get_pointer(source, std::true_type{}));
  }

  // clone the data.
  static void clone(any_data_t *dest, const any_data_t *source, std::false_type)
  {
    dest->remote_addr = ::new T(*get_pointer(source, std::false_type{}));
  }

  // move the data.
  static std::false_type
  move(any_data_t *dest, const any_data_t *source,
       std::true_type) noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    ::new (&dest->local_buffer)
        T(std::move(*get_pointer(source, std::true_type{})));
    return {};
  }

  // Note after move operation, the manage function of source should be manually
  // set to nullptr
  static std::true_type move(any_data_t *dest, const any_data_t *source,
                             std::false_type) noexcept
  {
    dest->remote_addr = get_pointer(source, std::false_type{});
    return {};
  }

  // destroy the data.
  static void destroy(any_data_t *dest, std::true_type)
  {
    get_pointer(dest, std::true_type{})->~T();
  }

  // destroy the data.
  static void destroy(any_data_t *dest, std::false_type)
  {
    delete static_cast<T *>(dest->remote_addr);
  }

  // the manage function.
  static void *manage(any_data_t *dest, const any_data_t *source,
                      any_manage_op op)
  {
    switch (op)
    {
    case any_manage_op::get_pointer:
      return get_pointer(source, local_storage);

#ifndef GPCL_CONFIG_NO_RTTI
    case any_manage_op::get_type_info:
      return const_cast<type_info *>(get_type_info(source));
#endif

    case any_manage_op::clone:
      clone(dest, source, local_storage);
      return nullptr;

    case any_manage_op::move:
      if (move(dest, source, local_storage))
        return dest;
      else
        return nullptr;

    case any_manage_op::destroy:
      destroy(dest, local_storage);
      return nullptr;

    default:
      GPCL_UNREACHABLE("invalid sbo op");
    }
  }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_ANY_MANAGER_HPP
