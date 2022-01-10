#ifndef GPCL_META_FWD_HPP
#define GPCL_META_FWD_HPP

#include <type_traits>
#include <utility>

namespace gpcl::meta
{

using std::integer_sequence;

template <template <typename...> class C, typename... Xs>
struct defer;

template <template <typename...> typename F>
struct quote;

template <typename... Fs>
struct compose;

template <typename F, typename... Xs>
struct bind_front;


template <typename F, typename... Ys>
struct bind_back;


}

#endif // GPCL_META_FWD_HPP
