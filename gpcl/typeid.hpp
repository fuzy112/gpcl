#pragma once

#include <gpcl/detail/config.hpp>

#if defined(GPCL_NO_RTTI)
#  include <gpcl/detail/typeid.hpp>
#else
#  include <typeinfo>
#endif

namespace gpcl {

#if defined(GPCL_NO_RTTI)
using type_info = detail::type_info;

template <typename T>
constexpr const type_info &typeid_() noexcept
{
  return detail::typeid_impl<T>::typeid_;
}

#else

using type_info = std::type_info;

template <typename T>
constexpr const type_info &typeid_() noexcept
{
  return typeid(T);
}

#endif

} // namespace gpcl
