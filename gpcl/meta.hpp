#ifndef GPCL_META_HPP
#define GPCL_META_HPP

#include <gpcl/detail/config.hpp>

#include <type_traits>
#include <utility>

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

template <typename Sequence>
struct front_;

template <template <typename...> class T, typename X, typename... Xs>
struct front_<T<X, Xs...>>
{
  using type = X;
};

template <typename Sequence>
using front = _t<front_<Sequence>>;

template <typename X, typename Y>
using eq = bool_<X::value == Y::value>;

template <typename X, typename Y>
using ne = not_<eq<X, Y>>;

template <typename Sequence, std::size_t N>
struct at_c_
{
};

template <template <typename...> class T, typename X, typename... Xs>
struct at_c_<T<X, Xs...>, 0>
{
  using type = X;
};

template <template <typename...> class T, typename X, typename... Xs,
          std::size_t N>
struct at_c_<T<X, Xs...>, N> : at_c_<T<Xs...>, N - 1>
{
};

template <typename Sequence, typename N>
using at_ = at_c_<Sequence, N::value>;

template <typename Sequence, typename N>
using at = _t<at_<Sequence, N>>;

template <typename Sequence, std::size_t N>
using at_c = at<Sequence, size_t<N>>;

template <typename Sequence>
using back = at_c<Sequence, Sequence::size() - 1>;

template <typename Sequence>
using size = meta::size_t<Sequence::size()>;

using npos = meta::size_t<std::size_t(-1)>;

template <typename Sequence>
using empty = bool_<size<Sequence>::value == 0>;

template <typename Sequence, typename X>
struct push_back_;

template <template <typename...> class T, typename... Xs, typename X>
struct push_back_<T<Xs...>, X>
{
  using type = T<Xs..., X>;
};

template <typename Sequence, typename X>
using push_back = _t<push_back_<Sequence, X>>;

template <typename Sequence, typename X>
struct push_front_;

template <template <typename...> class T, typename... Xs, typename X>
struct push_front_<T<Xs...>, X>
{
  using type = T<X, Xs...>;
};

template <typename Sequence, typename X>
using push_front = _t<push_front_<Sequence, X>>;

template <typename Sequence>
struct pop_back_;

template <template <typename...> class T, typename... Xs>
struct pop_back_<T<Xs...>>
{
  using _index_sequence = std::make_index_sequence<sizeof...(Xs) - 1>;

  template <typename Is>
  struct helper;

  template <std::size_t... Is>
  struct helper<std::index_sequence<Is...>>
  {
    using type = T<at_c<T<Xs...>, Is>...>;
  };

  using type = _t<helper<_index_sequence>>;
};

template <typename Sequence>
using pop_back = _t<pop_back_<Sequence>>;

template <typename Sequence>
struct pop_front_;

template <template <typename...> class T, typename X, typename... Xs>
struct pop_front_<T<X, Xs...>>
{
  using type = T<Xs...>;
};

template <typename Sequence>
using pop_front = _t<pop_front_<Sequence>>;

template <typename... Lists>
struct concat_;

template <>
struct concat_<>
{
  using type = list<>;
};

template <typename... Xs>
struct concat_<list<Xs...>>
{
  using type = list<Xs...>;
};

template <typename... Xs, typename... Ys, typename... Rest>
struct concat_<list<Xs...>, list<Ys...>, Rest...>
{
  using type = _t<concat_<list<Xs..., Ys...>, Rest...>>;
};

template <typename... Lists>
using concat = _t<concat_<Lists...>>;

template <template <typename...> class T, typename Sequence>
struct as_;

template <template <typename...> class T, template <typename...> class U,
          typename... Xs>
struct as_<T, U<Xs...>>
{
  using type = T<Xs...>;
};

template <template <typename...> class T, typename Sequence>
using as = _t<as_<T, Sequence>>;

template <typename Sequence>
using as_list = as<list, Sequence>;

template <typename Sequence>
using as_tuple = as<std::tuple, Sequence>;

template <typename N, typename Sequence, typename Y>
struct find_type_ : npos
{
};

template <template <typename...> class T, typename N, typename X,
          typename... Xs>
struct find_type_<N, T<X, Xs...>, X> : N
{
};

template <template <typename...> class T, typename N, typename X,
          typename... Xs, typename Y>
struct find_type_<N, T<X, Xs...>, Y>
    : find_type_<size_t<N::value + 1>, T<Xs...>, Y>
{
};

template <typename Sequence, typename Y>
using find_type = _t<find_type_<size_t<0>, Sequence, Y>>;

template <typename Sequence, typename F>
struct transform_;

template <template <typename...> class T, typename... Xs, typename F>
struct transform_<T<Xs...>, F>
{
  static_assert(sizeof(F));
  using type = T<invoke<F, Xs>...>;
};

template <typename Sequence, typename F>
using transform = _t<transform_<Sequence, F>>;

template <typename Param, typename Arg>
struct substitute
{
    template <typename Tree>
    struct invoke_
    {
      using type = std::conditional_t<std::is_same_v<Tree, Param>, Arg, Tree>;
    };

    template <typename X> struct invoke_<X &> {
      using type = _t<invoke_<X>> &;
    };

    template <typename X> struct invoke_<X &&> {
      using type = _t<invoke_<X>> &&;
    };

    template <typename X> struct invoke_<X *> {
      using type = _t<invoke_<X>> *;
    };

    template <typename X> struct invoke_<X const> {
      using type =_t<invoke_<X>> const;
    };

    template <typename X> struct invoke_<X volatile> {
      using type = _t<invoke_<X>> volatile;
    };

    template <typename X> struct invoke_<X volatile const> {
      using type = _t<invoke_<X>> const volatile;
    };

    template <template <typename ...> class T, typename ...Xs>
    struct invoke_<T<Xs...>>
    {
       using type = transform<T<Xs...>, substitute<Param, Arg>>;
    };

    template <typename ...Tree>
    using invoke = _t<invoke_<Tree...>>;
};

template <typename... Ts>
struct lambda
{
  using _params = pop_back<list<Ts...>>;
  using _body = back<list<Ts...>>;

  template <typename Args, typename T = std::make_index_sequence<Args::size()>>
  struct invoke_helper_;

  template <typename Args, std::size_t ...Is>
  struct invoke_helper_<Args, std::index_sequence<Is...>>
  {
    using type = compose<
        substitute<at_c<_params, Is>, at_c<Args, Is>>...
    >;
  };

  template <typename Args>
  using invoke_helper = _t<invoke_helper_<Args>>;

  template <typename... Xs>
  struct invoke_
  {
    using type = invoke<
      invoke_helper<list<Xs...>>,
      _body>;
  };

  template <typename... Xs>
  using invoke = _t<invoke_<Xs...>>;
};

} // namespace gpcl::meta

#endif // GPCL_META_HPP
