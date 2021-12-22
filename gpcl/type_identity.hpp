//
// type_identity.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_TYPE_IDENTITY_HPP
#define GPCL_TYPE_IDENTITY_HPP

namespace gpcl {

/// Used to establish non-deduced contexts in template deduction.
template <class T>
struct type_identity
{
  using type = T;
};

/// @relates gpcl::type_identity
template <class T>
using type_identity_t = typename type_identity<T>::type;

} // namespace gpcl

#endif // GPCL_TYPE_IDENTITY_HPP
