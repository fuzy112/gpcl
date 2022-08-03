//
// meta_fwd.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_META_FWD_HPP
#define GPCL_META_FWD_HPP

#include <gpcl/detail/config.hpp>

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
