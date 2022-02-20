#
# CMakeLists.txt
# ~~~~~~~~~~~~~~
#
# Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

add_custom_target(
  check-copyright
  ${CMAKE_COMMAND} "-DGPCL_SOURCE_DIR=${GPCL_SOURCE_DIR}" -P add_copyright.cmake
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(
  update-gpcl.hpp
  ${CMAKE_COMMAND} "-DGPCL_SOURCE_DIR=${GPCL_SOURCE_DIR}" -P
  generate_convenient_header.cmake
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(
  update-version
  ${CMAKE_COMMAND} 
  "-DGPCL_HOMEPAGE_URL=${GPCL_HOMEPAGE_URL}"
  "-DGPCL_SOURCE_DIR=${GPCL_SOURCE_DIR}"
  "-DGPCL_VERSION=${GPCL_VERSION}"
  "-DGPCL_VERSION_NUMBER=${GPCL_VERSION_NUMBER}"
  "-DGPCL_VERSION_MAJOR=${GPCL_VERSION_MAJOR}"
  "-DGPCL_VERSION_MINOR=${GPCL_VERSION_MINOR}"
  "-DGPCL_VERSION_PATCH=${GPCL_VERSION_PATCH}"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/update_version.cmake")