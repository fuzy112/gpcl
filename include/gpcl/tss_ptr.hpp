//
// tss_ptr.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_TSS_PTR_HPP
#define GPCL_TSS_PTR_HPP

#include <gpcl/detail/config.hpp>

#if defined(GPCL_POSIX)
#  include <gpcl/detail/posix_tss_ptr.hpp>
#else
#  include <gpcl/detail/keyword_tss_ptr.hpp>
#endif

namespace gpcl {

template <typename T>
#if defined(GPCL_POSIX)
using tss_ptr = detail::posix_tss_ptr<T>;
#else
using tss_ptr = detail::keyword_tss_ptr<T>;
#endif

} // namespace gpcl

#endif // GPCL_TSS_PTR_HPP
