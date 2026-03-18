//
// is_lockable.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IS_LOCKABLE_HPP
#define GPCL_IS_LOCKABLE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/is_basic_lockable.hpp>

namespace gpcl {

#ifndef GPCL_DOXYGEN
template <typename T, typename = void>
struct is_lockable : std::false_type
{
};

template <typename T>
struct is_lockable<
    T, typename std::enable_if<std::is_same<
           decltype(std::declval<T &>().try_lock()), bool>::value>::type>
    : is_basic_lockable<T>
{
};
#endif

} // namespace gpcl

#endif // GPCL_IS_LOCKABLE_HPP
