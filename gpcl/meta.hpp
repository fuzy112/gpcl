#ifndef GPCL_META_HPP
#define GPCL_META_HPP

#include <gpcl/meta_fwd.hpp>

namespace gpcl::meta {

template <typename T>
using _t = typename T::type;

template <typename...>
using void_ = void;

template <typename... Ts>
struct list;

namespace detail {

template <template <typename...> class C, typename, typename = void>
struct defer_
{
};

template <template <typename...> class C, template <typename...> class T,
          typename... Xs>
struct defer_<C, T<Xs...>, void_<C<Xs...>>>
{
  using type = C<Xs...>;
};

} // namespace detail

template <template <typename...> class C, typename... Xs>
struct defer : detail::defer_<C, list<Xs...>>
{
};

namespace lazy {
template <typename T>
using _t = defer<meta::_t, T>;
}

template <typename Callable, typename... Args>
using invoke = typename Callable::template invoke<Args...>;

namespace lazy {
template <typename F, typename... Xs>
using invoke = defer<F::template invoke, Xs...>;
}

template <template <typename...> typename F>
struct quote
{
  template <typename... Args>
  using invoke = _t<defer<F, Args...>>;
};

template <template <typename...> typename T>
using quote_trait = compose<quote<_t>, quote<T>>;

template <typename... Fs>
struct compose
{
};

template <typename F>
struct compose<F>
{
  // static_assert(is_callable_v<F>);

  template <typename... Args>
  using invoke = meta::invoke<F, Args...>;
};

template <typename F0, typename... Fs>
struct compose<F0, Fs...>
{
  // static_assert(is_callable_v<F0>);

  template <typename... Args>
  using invoke = meta::invoke<F0, meta::invoke<compose<Fs...>, Args...>>;
};

namespace lazy {
template <typename... Fs>
using compose = defer<meta::compose, Fs...>;
}

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

namespace lazy {
template <typename F, typename... Xs>
using bind_front = defer<meta::bind_front, F, Xs...>;

template <typename F, typename... Xs>
using bind_back = defer<meta::bind_back, F, Xs...>;
} // namespace lazy

using std::integral_constant;

template <bool V>
using bool_ = integral_constant<bool, V>;

template <std::size_t V>
using size_t = integral_constant<std::size_t, V>;

template <int V>
using int_ = integral_constant<int, V>;

template <char C>
using char_ = integral_constant<char, C>;

using npos = size_t<std::size_t(-1)>;

namespace detail {
template <typename T>
struct inc_;

template <typename T, T N>
struct inc_<integral_constant<T, N>> : integral_constant<T, N + 1>
{
};

template <typename T>
struct dec_;

template <typename T, T N>
struct dec_<integral_constant<T, N>> : integral_constant<T, N - 1>
{
};

} // namespace detail

template <typename T>
using inc = _t<detail::inc_<T>>;

template <typename T>
using dec = _t<detail::dec_<T>>;

template <typename T, typename U>
using eq = bool_<T::type::value == U::type::value>;

template <typename T, typename U>
using ne = bool_<T::type::value != U::type::value>;

namespace detail {
template <typename... Ts>
struct _if_;

template <typename C>
struct _if_<C> : std::enable_if<C::type::value>
{
};

template <typename C, typename T>
struct _if_<C, T> : std::enable_if<C::type::value, T>
{
};

template <typename C, typename T, typename F>
struct _if_<C, T, F> : std::conditional_t<C::type::value, T, F>
{
};
} // namespace detail

template <typename... Ts>
using if_ = _t<detail::_if_<Ts...>>;

template <typename... Xs>
using and_ = bool_<(true && ... && Xs::type::value)>;

template <typename... Xs>
using or_ = bool_<(false || ... || Xs::type::value)>;

template <typename X>
using not_ = bool_<!X::type::value>;

namespace lazy {
template <typename... Ts>
using if_ = defer<meta::if_, Ts...>;

template <typename... Xs>
using and_ = defer<meta::and_, Xs...>;

template <typename... Xs>
using or_ = defer<meta::or_, Xs...>;

template <typename X>
using not_ = defer<meta::not_, X>;
} // namespace lazy

namespace detail {
template <typename List, typename F>
struct apply_;

template <template <typename...> class T, typename... Xs, typename F>
struct apply_<T<Xs...>, F>
{
  using type = invoke<F, Xs...>;
};
} // namespace detail

template <typename F, typename List>
using apply = _t<detail::apply_<List, F>>;

namespace lazy {
template <typename F, typename List>
using apply = defer<meta::apply, F, List>;
}

namespace detail {
template <typename, typename State, typename F>
struct fold_
{
};

template <template <typename...> class C, typename State, typename F>
struct fold_<C<>, State, F>
{
  using type = State;
};

template <template <typename...> class C, typename X, typename... Xs,
          typename State, typename F>
struct fold_<C<X, Xs...>, State, F> : fold_<C<Xs...>, invoke<F, State, X>, F>
{
};
} // namespace detail

template <typename List, typename Init, typename F>
using fold = _t<detail::fold_<List, Init, F>>;

namespace lazy {
template <typename List, typename Init, typename F>
using fold = defer<meta::fold, List, Init, F>;
}

namespace detail {
template <typename, typename State, typename F>
struct foldr_;

template <template <typename...> class C, typename State, typename F>
struct foldr_<C<>, State, F>
{
  using type = State;
};

template <template <typename...> class C, typename X, typename... Xs,
          typename State, typename F>
struct foldr_<C<X, Xs...>, State, F>
    : invoke<F, _t<foldr_<C<Xs...>, State, F>>, X>
{
};

} // namespace detail

template <typename List, typename Init, typename F>
using foldr = _t<detail::foldr_<List, Init, F>>;

template <typename... Xs>
struct list
{
  using type = list;

  static constexpr std::size_t size() noexcept { return sizeof...(Xs); }
};

template <typename X, typename Y>
using pair = list<X, Y>;

namespace detail {

template <typename List>
struct front_
{
};

template <template <typename...> class C, typename X, typename... Xs>
struct front_<C<X, Xs...>>
{
  using type = X;
};
} // namespace detail

template <typename List>
using front = _t<detail::front_<List>>;

namespace detail {

template <typename List>
struct pop_front_
{
};

template <template <typename...> class C, typename X, typename... Xs>
struct pop_front_<C<X, Xs...>>
{
  using type = C<Xs...>;
};
} // namespace detail

template <typename List>
using pop_front = _t<detail::pop_front_<List>>;

namespace detail {
template <typename List, typename X>
struct push_front_;

template <template <typename...> class C, typename... Xs, typename X>
struct push_front_<C<Xs...>, X>
{
  using type = C<X, Xs...>;
};

} // namespace detail

template <typename List, typename X>
using push_front = _t<detail::push_front_<List, X>>;

namespace detail {

template <typename List>
struct back_
{
};

template <template <typename...> class C, typename X>
struct back_<C<X>>
{
  using type = X;
};

template <template <typename...> class C, typename... Xs, typename X>
struct back_<C<X, Xs...>>
{
  using type = _t<back_<C<Xs...>>>;
};

} // namespace detail

template <typename List>
using back = _t<detail::back_<List>>;

namespace detail {

template <typename List>
struct pop_back_
{
};

template <template <typename...> class C, typename X>
struct pop_back_<C<X>>
{
  using type = C<>;
};

template <template <typename...> class C, typename... Xs, typename X>
struct pop_back_<C<X, Xs...>>
{
  using type = push_front<_t<pop_back_<C<Xs...>>>, X>;
};

} // namespace detail

template <typename List>
using pop_back = _t<detail::pop_back_<List>>;

namespace detail {
template <typename List, typename X>
struct push_back_;

template <template <typename...> class C, typename... Xs, typename X>
struct push_back_<C<Xs...>, X>
{
  using type = C<Xs..., X>;
};
} // namespace detail

template <typename List, typename X>
using push_back = _t<detail::push_back_<List, X>>;

template <typename List>
using first = front<List>;

template <typename List>
using second = front<pop_front<List>>;

namespace detail {
template <typename T>
struct find_index_helper
{
  template <typename State, typename X>
  using invoke = pair<inc<front<State>>,
                      if_<std::is_same<X, T>, front<State>, second<State>>>;
};
} // namespace detail

template <typename List, typename X>
using find_index =
    second<fold<List, pair<size_t<0>, npos>, detail::find_index_helper<X>>>;

namespace detail {

template <typename F>
struct transform_fn
{
  template <typename State, typename X>
  using invoke = push_back<State, invoke<F, X>>;
};

} // namespace detail

template <typename List, typename F>
using transform = fold<List, list<>, detail::transform_fn<F>>;

namespace detail {
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

template <typename... Xs, typename... Ys, typename... Lists>
struct concat_<list<Xs...>, list<Ys...>, Lists...>
    : concat_<list<Xs..., Ys...>, Lists...>
{
};
} // namespace detail

template <typename... Lists>
using concat = _t<detail::concat_<Lists...>>;

template <typename ListOfLists>
using join = apply<quote<concat>, ListOfLists>;

namespace detail {
template <typename List, typename N, bool = true>
struct at_
{
};

template <typename X, typename... Xs, typename N>
struct at_<list<X, Xs...>, N, bool(N::type::value == 0)>
{
  using type = X;
};

template <typename X, typename... Xs, typename N>
struct at_<list<X, Xs...>, N, bool(N::type::value != 0)>
{
  using type =
      _t<at_<list<Xs...>, integral_constant<typename N::type::value_type,
                                            N::type::value - 1>>>;
};

} // namespace detail

template <typename List, typename N>
using at = _t<detail::at_<List, N>>;

template <typename List, std::size_t N>
using at_c = at<List, size_t<N>>;

template <typename List>
using size = size_t<List::size()>;

template <typename T>
struct id
{
  template <typename...>
  using invoke = T;

  using type = T;
};

template <typename Param, typename Arg>
struct substitute
{
  template <typename T>
  struct protect_
  {
    using type = list<id<T>>;
  };

  template <template <typename...> class C, typename... Ts>
  struct protect_<C<Ts...>>
  {
    using type = list<quote<C>, Ts...>;
  };

  template <typename T>
  using protect = _t<protect_<T>>;

  template <typename T>
  using unprotect = apply<quote<invoke>, T>;

  template <typename Tree>
  struct invoke_
  {
    using type = std::conditional_t<std::is_same_v<Tree, Param>, Arg, Tree>;
  };

  template <typename X>
  struct invoke_<X &>
  {
    using type = _t<invoke_<X>> &;
  };

  template <typename X>
  struct invoke_<X &&>
  {
    using type = _t<invoke_<X>> &&;
  };

  template <typename X>
  struct invoke_<X *>
  {
    using type = _t<invoke_<X>> *;
  };

  template <typename X>
  struct invoke_<X const>
  {
    using type = _t<invoke_<X>> const;
  };

  template <typename X>
  struct invoke_<X volatile>
  {
    using type = _t<invoke_<X>> volatile;
  };

  template <typename X>
  struct invoke_<X volatile const>
  {
    using type = _t<invoke_<X>> const volatile;
  };

  template <template <typename...> class T, typename... Xs>
  struct invoke_<T<Xs...>>
  {
    using type =
        unprotect<transform<protect<T<Xs...>>, substitute<Param, Arg>>>;
  };

  template <typename... Tree>
  using invoke = _t<invoke_<Tree...>>;
};

template <typename... Ts>
struct lambda
{
  using _params = pop_back<list<Ts...>>;
  using _body = back<list<Ts...>>;

  template <typename Args, typename T = std::make_index_sequence<Args::size()>>
  struct invoke_helper_;

  template <typename Args, std::size_t... Is>
  struct invoke_helper_<Args, std::index_sequence<Is...>>
  {
    using type = compose<substitute<at_c<_params, Is>, at_c<Args, Is>>...>;
  };

  template <typename Args>
  using invoke_helper = _t<invoke_helper_<Args>>;

  template <typename... Xs>
  struct invoke_
  {
    using type = invoke<invoke_helper<list<Xs...>>, _body>;
  };

  template <typename... Xs>
  using invoke = _t<invoke_<Xs...>>;
};

namespace detail {
template <typename State, typename List>
struct cartesian_product_fn
{
  template <typename X>
  struct lambda0
  {
    template <typename Xs>
    using lambda1 = list<push_front<Xs, X>>;
    using type = join<transform<State, quote<lambda1>>>;
  };

  using type = join<transform<List, quote_trait<lambda0>>>;
};
} // namespace detail

template <typename ListOfLists>
using cartesian_product =
    meta::foldr<ListOfLists, meta::list<meta::list<>>,
                meta::quote_trait<detail::cartesian_product_fn>>;

namespace detail {
template <typename T>
struct as_list_;

template <template <typename...> class C, typename... Xs>
struct as_list_<C<Xs...>>
{
  using type = list<Xs...>;
};
} // namespace detail

template <typename T>
using as_list = _t<detail::as_list_<T>>;

template <typename List>
using as_tuple = apply<quote<std::tuple>, List>;

} // namespace gpcl::meta

#endif // GPCL_META_HPP
