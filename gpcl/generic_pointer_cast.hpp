//
// generic_pointer_cast.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_GENERIC_POINTER_CAST_HPP
#define GPCL_GENERIC_POINTER_CAST_HPP

namespace gpcl {

/// @ingroup SmartPtr
/// @{
template <class T, class U>
T *static_pointer_cast(U *p) noexcept
{
  return static_cast<T *>(p);
}

template <class T, class U>
T *dynamic_pointer_cast(U *p) noexcept
{
  return dynamic_cast<T *>(p);
}

template <class T, class U>
T *const_pointer_cast(U *p) noexcept
{
  return const_cast<T *>(p);
}

template <class T, class U>
T *reinterpret_pointer_cast(U *p) noexcept
{
  return reinterpret_cast<T *>(p);
}
/// @}

} // namespace gpcl

#endif // GPCL_GENERIC_POINTER_CAST_HPP
