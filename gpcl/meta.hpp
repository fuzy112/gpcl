#ifndef GPCL_META_HPP
#define GPCL_META_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>

namespace gpcl::meta {

template <typename T, typename Enable = void>
struct is_callable : std::false_type
{
};

template <typename T>
struct is_callable<T, std::void_t<typename T::template invoke<void>>>
    : std::true_type
{
};

template <typename T>
inline constexpr bool is_callable_v = is_callable<T>::value;

template <typename T>
using _t = typename T::type;

template <typename Callable, typename... Args>
using invoke = typename Callable::template invoke<Args...>;

template <template <typename...> typename F>
struct quote
{
  template <typename... Args>
  using invoke = F<Args...>;
};

template <template <typename...> typename T>
struct quote_trait
{
  template <typename... Args>
  using invoke = _t<T<Args...>>;
};

template <typename... Fs>
struct compose
{
};

template <typename F>
struct compose<F>
{
  static_assert(is_callable_v<F>);

  template <typename... Args>
  using invoke = meta::invoke<F, Args...>;
};

template <typename F0, typename F1, typename... Fs>
struct compose<F0, F1, Fs...>
{
  template <typename... Args>
  using invoke = meta::invoke<F0, meta::invoke<compose<F1, Fs...>, Args...>>;
};

template <typename F, typename... Xs>
struct bind_front
{
  template <typename... Ys>
  using invoke = meta::invoke<F, Xs..., Ys...>;
};

template <typename F, typename... Ys>
struct bind_back
{
  template <typename... Xs>
  using invoke = meta::invoke<F, Xs..., Ys...>;
};

using std::integral_constant;

template <bool V>
using bool_ = integral_constant<bool, V>;

template <std::size_t V>
using size_t = integral_constant<std::size_t, V>;

template <bool C, typename T, typename F>
struct if_c_;

template <typename T, typename F>
struct if_c_<true, T, F>
{
  using type = T;
};

template <typename T, typename F>
struct if_c_<false, T, F>
{
  using type = F;
};

template <bool C, typename T, typename F>
using if_c = _t<if_c_<C, T, F>>;

template <typename C, typename T, typename F>
using if_ = if_c<C::value, T, F>;

template <typename X1, typename... Xs>
using and_ = bool_<(X1::value && ... && Xs::value)>;

template <typename X1, typename... Xs>
using or_ = bool_<(X1::value || ... || Xs::value)>;

template <typename X>
using not_ = bool_<!X::value>;

template <typename... Xs>
struct list
{
  static constexpr std::size_t size() noexcept { return sizeof...(Xs); }
};

template <typename List>
struct front_;

template <typename X, typename... Xs>
struct front_<list<X, Xs...>>
{
  using type = X;
};

template <typename List>
using front = _t<front_<List>>;

template <typename X, typename Y>
using eq = bool_<X::value == Y::value>;

template <typename X, typename Y>
using ne = not_<eq<X, Y>>;

template <typename List, std::size_t N>
struct at_c_
{
};

template <typename X, typename... Xs>
struct at_c_<list<X, Xs...>, 0>
{
  using type = X;
};

template <typename X, typename... Xs, std::size_t N>
struct at_c_<list<X, Xs...>, N> : at_c_<list<Xs...>, N - 1>
{
};

template <typename List, typename N>
using at_ = at_c_<List, N::value>;

template <typename List, typename N>
using at = _t<at_<List, N>>;

template <typename List, std::size_t N>
using at_c = at<List, size_t<N>>;

template <typename List>
using back = at_c<List, List::size() - 1>;

template <typename List>
using size = meta::size_t<List::size()>;

using npos = meta::size_t<std::size_t(-1)>;

template <typename List>
using empty = bool_<size<List>::value == 0>;

} // namespace gpcl::meta

#endif // GPCL_META_HPP
