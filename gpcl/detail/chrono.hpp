//
// chrono.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_CHRONO_HPP
#define GPCL_DETAIL_CHRONO_HPP

#include <gpcl/detail/config.hpp>

#ifndef GPCL_USE_BOOST_CHRONO
#  include <chrono>
#else
#  include <boost/chrono.hpp>
#endif

namespace gpcl {
namespace detail {

#ifndef GPCL_USE_BOOST_CHRONO
namespace chrono = std::chrono;
#else
namespace chrono = boost::chrono;
#endif

} // namespace detail
} // namespace gpcl

#endif
