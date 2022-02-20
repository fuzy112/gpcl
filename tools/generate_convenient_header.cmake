#
# generate_convenient_header.cmake
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

set(INCLUDES "")

file(GLOB files "${GPCL_SOURCE_DIR}/include/gpcl/*.hpp")
foreach(file ${files})
  get_filename_component(filename ${file} NAME)
  if(NOT filename STREQUAL "ext.hpp")
    string(APPEND INCLUDES "#include <gpcl/${filename}>\n")
  endif()
endforeach(file ${files})

configure_file("${CMAKE_CURRENT_LIST_DIR}/gpcl.hpp.in"
               "${GPCL_SOURCE_DIR}/include/gpcl.hpp")
