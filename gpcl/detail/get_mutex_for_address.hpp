//
// get_mutex_for_address.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_GET_MUTEX_FOR_ADDRESS_HPP
#define GPCL_DETAIL_GET_MUTEX_FOR_ADDRESS_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/shared_ptr.hpp>

namespace gpcl ::detail {


#if GPCL_DETAIL_MUTEX_FOR_ADDRESS_USE_SHARED_PTR
typedef shared_ptr<mutex> mutex_for_address_ptr;
#else
typedef mutex *mutex_for_address_ptr;
#endif

GPCL_DECL
mutex_for_address_ptr get_mutex_for_address(void const *key);

} // namespace gpcl::detail

#if defined GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/get_mutex_for_address.ipp>
#endif

#endif // GPCL_DETAIL_GET_MUTEX_FOR_ADDRESS_HPP
