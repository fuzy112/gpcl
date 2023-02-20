#ifndef GPCL_DETAIL_ATOMIC_FACADE_HPP
#define GPCL_DETAIL_ATOMIC_FACADE_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/memory_order.hpp>
#include <gpcl/noncopyable.hpp>

#include <type_traits>

namespace gpcl {
namespace detail {

template <typename Derived, typename ValueType = typename Derived::value_type,
          typename DifferenceType = ValueType,
          typename UnderlyingType = ValueType>
struct atomic_facade
{
  typedef ValueType value_type;

  typedef DifferenceType difference_type;

  Derived *derived() noexcept { return static_cast<Derived *>(this); }

  Derived volatile *derived() volatile noexcept
  {
    return static_cast<Derived volatile *>(this);
  }

  Derived const *derived() const noexcept
  {
    return static_cast<Derived const *>(this);
  }

  Derived const volatile *derived() const volatile noexcept
  {
    return static_cast<Derived const volatile *>(this);
  }

  value_type operator=(value_type desired) &noexcept
  {
    derived()->store(desired);
    return desired;
  }

  value_type operator=(value_type desired) volatile &noexcept
  {
    derived()->store(desired);
    return desired;
  }

  operator value_type() const noexcept { return derived()->load(); }

  operator value_type() const volatile noexcept { return derived()->load(); }

  value_type operator++() noexcept { return derived()->fetch_add(1) + 1; }

  value_type operator++() volatile noexcept
  {
    return derived()->fetch_add(1) + 1;
  }

  value_type operator++(int) noexcept { return derived()->fetch_add(1); }

  value_type operator++(int) volatile noexcept
  {
    return derived()->fetch_add(1);
  }

  value_type operator--() noexcept { return derived()->fetch_sub(1) - 1; }

  value_type operator--() volatile noexcept
  {
    return derived()->fetch_sub(1) - 1;
  }

  value_type operator--(int) noexcept { return derived()->fetch_sub(1); }

  value_type operator--(int) volatile noexcept
  {
    return derived()->fetch_sub(1);
  }

  value_type operator+=(difference_type arg) noexcept
  {
    return derived()->fetch_add(arg) + arg;
  }

  value_type operator+=(difference_type arg) volatile noexcept
  {
    return derived()->fetch_add(arg) + arg;
  }

  value_type operator-=(difference_type arg) noexcept
  {
    return derived()->fetch_sub(arg) - arg;
  }

  value_type operator-=(difference_type arg) volatile noexcept
  {
    return derived()->fetch_sub(arg) - arg;
  }

  value_type operator&=(value_type arg) noexcept
  {
    return derived()->fetch_and(arg) & arg;
  }

  value_type operator&=(value_type arg) volatile noexcept
  {
    return derived()->fetch_and(arg) & arg;
  }

  value_type operator|=(value_type arg) noexcept
  {
    return derived()->fetch_or(arg) | arg;
  }

  value_type operator|=(value_type arg) volatile noexcept
  {
    return derived()->fetch_or(arg) | arg;
  }

  value_type operator^=(value_type arg) noexcept
  {
    return derived()->fetch_xor(arg) ^ arg;
  }

  value_type operator^=(value_type arg) volatile noexcept
  {
    return derived()->fetch_xor(arg) ^ arg;
  }

  value_type fetch_sub(difference_type arg,
                       memory_order order = memory_order::seq_cst) noexcept
  {
    return derived()->fetch_add(
        -static_cast<typename std::make_signed<difference_type>::type>(arg),
        order);
  }

  value_type
  fetch_sub(difference_type arg,
            memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return derived()->fetch_add(
        -static_cast<typename std::make_signed<difference_type>::type>(arg),
        order);
  }

  bool
  compare_exchange_weak(value_type &expected, value_type desired,
                        memory_order order = memory_order::seq_cst) noexcept
  {
    return derived()->compare_exchange_strong(expected, desired, order);
  }

  bool compare_exchange_weak(
      value_type &expected, value_type desired,
      memory_order order = memory_order::seq_cst) volatile noexcept
  {
    return derived()->compare_exchange_strong(expected, desired, order);
  }
};

} // namespace detail
} // namespace gpcl

#endif
