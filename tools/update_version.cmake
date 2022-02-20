#
# update_version.cmake
# ~~~~~~~~~~~~~~~~~~~~
#
# Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

set(GPCL_VERSION_STRING \"${GPCL_VERSION}\")
configure_file("${CMAKE_CURRENT_LIST_DIR}/version.hpp.in"
               "${GPCL_SOURCE_DIR}/include/gpcl/version.hpp")
configure_file("${CMAKE_CURRENT_LIST_DIR}/gpcl.spec.in"
               "${CMAKE_CURRENT_LIST_DIR}/gpcl.spec")
