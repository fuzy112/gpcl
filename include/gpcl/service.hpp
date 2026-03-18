//
// service.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SERVICE_HPP
#define GPCL_SERVICE_HPP

#include <gpcl/detail/config.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/posix_service.hpp>
#endif

namespace gpcl {

#if defined GPCL_POSIX
using service = detail::posix_service;
#endif

} // namespace gpcl

#endif // GPCL_SERVICE_HPP
