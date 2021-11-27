//
// config.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EXT_DETAIL_CONFIG_HPP
#define GPCL_EXT_DETAIL_CONFIG_HPP

#include <gpcl/detail/config.hpp>

#if !defined(GPCL_EXT_ENABLE_MOSQUITTO) && __has_include(<mosquitto.h>)
#  define GPCL_EXT_ENABLE_MOSQUITTO 1
#endif

#if !defined(GPCL_EXT_ENABLE_SQLITE) && __has_include(<sqlite3.h>)
#  define GPCL_EXT_ENABLE_SQLITE 1
#endif

#endif
