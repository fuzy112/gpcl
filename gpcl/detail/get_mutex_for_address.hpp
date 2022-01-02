#ifndef GPCL_DETAIL_GET_MUTEX_FOR_ADDRESS_HPP
#define GPCL_DETAIL_GET_MUTEX_FOR_ADDRESS_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/shared_ptr.hpp>

namespace gpcl ::detail {

GPCL_DECL
shared_ptr<mutex> get_mutex_for_address(void const *key);

} // namespace gpcl::detail

#if defined GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/get_mutex_for_address.ipp>
#endif

#endif // GPCL_DETAIL_GET_MUTEX_FOR_ADDRESS_HPP
