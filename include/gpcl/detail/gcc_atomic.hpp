#ifndef GPCL_DETAIL_GCC_ATOMIC_HPP
#define GPCL_DETAIL_GCC_ATOMIC_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/detail/atomic_facade.hpp>

#ifdef GPCL_LINUX
#  include <gpcl/detail/futex_atomic_wait.hpp>
#endif

namespace gpcl {
namespace detail {

inline int gcc_memory_order(memory_order order) noexcept
{
  switch (order)
  {
  case memory_order::relaxed:
    return __ATOMIC_RELAXED;
  case memory_order::consume:
    return __ATOMIC_CONSUME;
  case memory_order::acquire:
    return __ATOMIC_ACQUIRE;
  case memory_order::release:
    return __ATOMIC_RELEASE;
  case memory_order::acq_rel:
    return __ATOMIC_ACQ_REL;
  case memory_order::seq_cst:
    return __ATOMIC_SEQ_CST;
  default:
    GPCL_UNREACHABLE("Invalid memory order");
  }
}

template <typename T, typename DifferenceType = T, typename V = void>
struct gcc_atomic
    : atomic_facade<gcc_atomic<T, DifferenceType, V>, DifferenceType>
{
  typedef T value_type;
  typedef DifferenceType difference_type;

  T value;

  constexpr gcc_atomic() noexcept(
      std::is_nothrow_default_constructible<T>::value)
      : value()
  {
  }

  constexpr gcc_atomic(T desired) noexcept : value(desired) {}

  void store(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    __atomic_store(&value, &desired, gcc_memory_order(order));
  }

  T load(memory_order order = memory_order::seq_cst) const volatile noexcept
  {
    T result;
    __atomic_load(&value, &result, gcc_memory_order(order));
    return result;
  }

  T exchange(T desired,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    T result;
    __atomic_exchange(&value, &desired, &result, gcc_memory_order(order));
    return result;
  }

  bool compare_exchange_strong(T &expected, T desired, memory_order success,
                               memory_order failure) volatile noexcept
  {
    return __atomic_compare_exchange(&value, &expected, &desired, false,
                                     gcc_memory_order(success),
                                     gcc_memory_order(failure));
  }

  bool compare_exchange_strong(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept
  {
    memory_order failure = order;
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_strong(expected, desired, order, failure);
  }

  bool compare_exchange_weak(T &expected, T desired, memory_order success,
                             memory_order failure) volatile noexcept
  {
    return __atomic_compare_exchange(&value, &expected, &desired, true,
                                     gcc_memory_order(success),
                                     gcc_memory_order(failure));
  }

  bool compare_exchange_weak(
      T &expected, T desired,
      memory_order order = memory_order::seq_cst) volatile noexcept
  {
    memory_order failure = order;
    if (order == memory_order::release)
      failure = memory_order::relaxed;
    else if (order == memory_order::acq_rel)
      failure = memory_order::acquire;
    return compare_exchange_weak(expected, desired, order, failure);
  }

  T fetch_add(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_add(&value, arg, gcc_memory_order(order));
  }

  T fetch_sub(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_sub(&value, arg, gcc_memory_order(order));
  }

  T fetch_and(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_and(&value, arg, gcc_memory_order(order));
  }

  T fetch_xor(T arg,
              memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_xor(&value, arg, gcc_memory_order(order));
  }

  T fetch_or(T arg,
             memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return __atomic_fetch_or(&value, arg, gcc_memory_order(order));
  }

  static constexpr bool is_always_lock_free =
      __atomic_always_lock_free(sizeof(T), 0);

  bool is_lock_free() const volatile noexcept
  {
    return __atomic_is_lock_free(sizeof(T), &value);
  }

  void wait(T old, memory_order order = memory_order::seq_cst) volatile noexcept
  {
    futex_atomic_wait_on_address((T *)&value, old, gcc_memory_order(order));
  }

  void notify_all() volatile noexcept
  {
    futex_atomic_wake_by_address((T *)&value, true);
  }

  void notify_one() volatile noexcept
  {
    futex_atomic_wake_by_address((T *)&value, false);
  }
};

} // namespace detail
} // namespace gpcl

#endif //
