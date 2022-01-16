#ifndef GPCL_LOCK_HPP
#define GPCL_LOCK_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/is_basic_lockable.hpp>
#include <gpcl/is_lockable.hpp>
#include <gpcl/unique_lock.hpp>
#include <gpcl/scope_success.hpp>

#include <tuple>
#include <type_traits>

namespace gpcl {
namespace detail {

template <typename M1>
int try_lock_impl(int index, M1 &m1)
{
  if (m1.try_lock())
  {
    return -1;
  }
  return index;
}

template <typename M1, typename M2, typename... Ms>
int try_lock_impl(int index, M1 &m1, M2 &m2, Ms &...ms)
{
  unique_lock lock1(m1, try_to_lock);
  if (lock1.owns_lock())
  {
    index = try_lock_impl(index + 1, m2, ms...);
    if (-1 == index)
    {
      lock1.release();
    }
  }
  return index;
}

template <typename M1, typename M2>
bool attempt_lock_two(M1 &m1, M2 &m2)
{
  unique_lock lock1(m1);
  if (m2.try_lock())
  {
    lock1.release();
    return true;
  }
  return false;
}

template <typename M1, typename M2>
void lock_impl(M1 &m1, M2 &m2)
{
  while (!attempt_lock_two(m1, m2) && !attempt_lock_two(m2, m1))
  {
  }
}

template <typename IntegerSequence, typename N>
struct integer_sequence_add;

template <typename Type, Type... Vals, typename N>
struct integer_sequence_add<std::integer_sequence<Type, Vals...>, N>
{
  using type =
      std::integer_sequence<decltype(std::declval<Type>() +
                                     std::declval<typename N::value_type>()),
                            (Vals + N::type::value)...>;
};

template <typename IntegerSequence, typename N>
using integer_sequence_add_t =
    typename integer_sequence_add<IntegerSequence, N>::type;

template <typename Type, Type Begin, Type End>
using integer_sequence_range =
    integer_sequence_add_t<std::make_integer_sequence<Type, End - Begin>,
                           std::integral_constant<Type, Begin>>;

template <typename... Ms>
bool try_lock_range_helper(std::integer_sequence<int>, std::tuple<Ms &...> ms)
{
  return true;
}

template <int I, int... Is, typename... Ms>
bool try_lock_range_helper(std::integer_sequence<int, I, Is...>,
                           std::tuple<Ms &...> ms)
{
  unique_lock lock_I(std::get<I>(ms), try_to_lock);
  if (lock_I.owns_lock())
  {
    if (try_lock_range(std::integer_sequence<int, Is...>(), ms))
    {
      lock_I.release();
      return true;
    }
  }
  return false;
}

template <int Begin, int End, typename... Ms>
bool try_lock_range(std::tuple<Ms &...> ms)
{
  return try_lock_range_helper(integer_sequence_range<int, Begin, End>(), ms);
}

template <int N, typename... Ms>
bool attempt_lock_n_first(std::tuple<Ms &...> ms)
{
  unique_lock lock1(std::get<N>(ms));

  if (try_lock_range<0, N>(ms) && try_lock_range<N + 1, sizeof...(Ms)>(ms))
  {
    lock1.release();
    return true;
  }

  return false;
}

template <int... Is, typename... Ms>
bool attempt_lock_many_helper(std::integer_sequence<int, Is...>,
                              std::tuple<Ms &...> ms)
{
  return (attempt_lock_n_first<Is>(ms) || ...);
}

template <typename... Ms>
bool attempt_lock_many(Ms &...ms)
{
  return attempt_lock_many_helper(
      std::make_integer_sequence<int, sizeof...(Ms)>(), std::tie(ms...));
}

template <typename M1, typename M2, typename M3, typename... Ms>
void lock_impl(M1 &m1, M2 &m2, M3 &m3, Ms &...ms)
{
  while (!attempt_lock_many(m1, m2, m3, ms...))
  {
  }
}

template <typename M1>
void lock_impl(M1 &m1)
{
  m1.lock();
}

} // namespace detail

template <typename M1, typename... Ms>
auto lock(M1 &m1, Ms &...ms)
    -> std::enable_if_t<is_lockable<M1>::value &&
                            (is_lockable<Ms>::value && ...) ||
                        (sizeof...(Ms) == 0 && is_basic_lockable<M1>::value)>
{
  detail::lock_impl(m1, ms...);
}

template <typename M1, typename... Ms>
auto try_lock(M1 &m1, Ms &...ms)
    -> std::enable_if_t<is_lockable<M1>::value &&
                            (is_lockable<Ms>::value && ...) ||
                        (sizeof...(Ms) == 0 && is_basic_lockable<M1>::value)>
{
  return detail::try_lock_impl(0, m1, ms...);
}

template <typename M1, typename... Ms>
auto unlock(M1 &m1, Ms &...ms)
    -> std::enable_if_t<is_lockable<M1>::value &&
                            (is_lockable<Ms>::value && ...) ||
                        (sizeof...(Ms) == 0 && is_basic_lockable<M1>::value)>
{
  m1.unlock();
  (ms.unlock(), ...);
}

} // namespace gpcl

#endif // GPCL_LOCK_HPP
