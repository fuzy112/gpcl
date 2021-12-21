//
// allocate_shared.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ALLOCATE_SHARED_HPP
#define GPCL_ALLOCATE_SHARED_HPP

#include <gpcl/detail/config.hpp>


namespace gpcl {

template <typename T> class shared_ptr;

/// @ingroup SmartPtr
template <typename T, typename Alloc, typename... Args>
shared_ptr<T> allocate_shared(const Alloc &alloc, Args &&...args);

} // namespace gpcl

#include <gpcl/impl/allocate_shared.hpp>

#endif // GPCL_ALLOCATE_SHARED_HPP
