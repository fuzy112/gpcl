#
# CMakeLists.txt
# ~~~~~~~~~~~~~~
#
# Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.16)

project(GPCL CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
include(CheckIPOSupported)
include(CTest)
include(FetchContent)
include(CMakePackageConfigHelpers)

option(GPCL_ENABLE_LTO "Enable link time optimisation" OFF)
option(GPCL_BUILD_TESTING "enable testing support" ON)
option(GPCL_BUILD_DOC "enable document generation" ON)
option(GPCL_DOC_LINK_TO_CPPREFERENCE_ONLINE
       "link to external cppreference documentation" TRUE)
option(GPCL_ENABLE_PCH "Enable precompiled headers" OFF)
option(GPCL_ENABLE_LLVM "Link to LLVM if found" TRUE)
option(GPCL_LINK_TO_LLVM_SO "Link to libLLVM.so rather than LLVMSymbolize"
       FALSE)
option(GPCL_ENABLE_BFD "Link to BFD if found" TRUE)
option(BUILD_SHARED_LIBS "Create shared libraies" TRUE)

check_ipo_supported(RESULT ipo_supported)
if(ipo_supported AND GPCL_ENABLE_LTO)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(NOT DEFINED BUILD_TESTING)
  set(BUILD_TESTING ${GPCL_BUILD_TESTING})
endif()

find_package(Threads)
find_package(LLVM)
find_package(Doxygen)
find_package(BFD)
find_package(Backtrace)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(gpcl src/src.cpp)
add_library(gpcl::gpcl ALIAS gpcl)

target_compile_features(gpcl PUBLIC cxx_std_17)

target_include_directories(
  gpcl PUBLIC $<BUILD_INTERFACE:${GPCL_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(Threads_FOUND)
  target_link_libraries(gpcl PUBLIC Threads::Threads)
endif()

if(LLVM_FOUND AND "${GPCL_ENABLE_LLVM}")
  target_include_directories(gpcl PUBLIC ${LLVM_INCLUDE_DIRS})
  if(NOT GPCL_LINK_TO_LLVM_SO)
    target_link_libraries(gpcl PUBLIC LLVMSymbolize)
  else()
    target_link_libraries(gpcl PUBLIC LLVM)
  endif()
  target_compile_definitions(gpcl PUBLIC GPCL_LLVM=1)
  set(GPCL_LLVM
      TRUE
      CACHE BOOL "")
elseif(BFD_FOUND AND "${GPCL_ENABLE_BFD}")
  target_link_libraries(gpcl PUBLIC BFD::BFD)
  target_compile_definitions(gpcl PUBLIC GPCL_BFD=1)
  set(GPCL_BFD
      TRUE
      CACHE BOOL "")
endif()

if(Backtrace_FOUND)
  target_link_libraries(gpcl PUBLIC "${Backtrace_LIBRARIES}")
  target_include_directories(gpcl PUBLIC "${Backtrace_INCLUDE_DIRS}")
  target_compile_definitions(gpcl
                             PUBLIC GPCL_BACKTRACE_HEADER="${Backtrace_HEADER}")
endif()

if(UNIX AND NOT APPLE)
  target_link_libraries(gpcl PUBLIC rt)
endif()

if(WIN32)
  target_link_libraries(gpcl PUBLIC DbgHelp)
endif()

if(MINGW)
  target_compile_options(gpcl INTERFACE "-Wa,-mbig-obj")
  target_link_libraries(gpcl PUBLIC)
endif()

if(GPCL_ENABLE_PCH)
  target_precompile_headers(gpcl INTERFACE $<BUILD_INTERFACE:<gpcl.hpp>>)
endif()

target_compile_features(gpcl PUBLIC cxx_std_17)

get_target_property(library_type gpcl TYPE)

if("${library_type}" STREQUAL "SHARED_LIBRARY")
  target_compile_definitions(gpcl PUBLIC GPCL_DYN_LINK=1)
endif()

set_target_properties(
  gpcl
  PROPERTIES SOVERSION 600
             VERSION 0.6.0
             DEFINE_SYMBOL GPCL_SOURCE)

FetchContent_Declare(
  cppreference
  URL https://upload.cppreference.com/mwiki/images/1/16/html_book_20190607.tar.xz
  URL_HASH
    SHA256=8f97b2baa749c748a2e022d785f1a2e95aa851a3075987dfcf38baf65e0e486d)

if(Doxygen_FOUND AND GPCL_BUILD_DOC)
  FetchContent_MakeAvailable(cppreference)
  if(GPCL_DOC_LINK_TO_CPPREFERENCE_ONLINE)
    list(
      APPEND
      DOXYGEN_TAGFILES
      "${cppreference_SOURCE_DIR}/cppreference-doxygen-web.tag.xml=http://en.cppreference.com/w/"
    )
  else()
    list(
      APPEND
      DOXYGEN_TAGFILES
      "${cppreference_SOURCE_DIR}/cppreference-doxygen-local.tag.xml=${cppreference_SOURCE_DIR}/reference/en/"
    )
  endif()
  set(DOXYGEN_PREDEFINED GPCL_DOXYGEN)

  set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  # set (DOXYGEN_EXCLUDE_SYMBOLS "*::detail::*;*_detail::*")
  set(DOXYGEN_STRIP_FROM_INC_PATH "include")
  doxygen_add_docs(doc README.md src include)

  find_package(
    Python
    COMPONENTS Interpreter
    QUIET)
  if(Python_FOUND)
    add_custom_target(
      serv_doc
      ${Python_EXECUTABLE} -m http.server 3333
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
      USES_TERMINAL)
  endif()
endif()

add_subdirectory(tools)

add_subdirectory(examples)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

configure_package_config_file(
  gpcl-config.cmake.in gpcl-config.cmake
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gpcl"
  PATH_VARS CMAKE_INSTALL_LIBDIR)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gpcl-config.cmake"
              "cmake/FindBFD.cmake" "cmake/FindIberty.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gpcl)

install(DIRECTORY include/gpcl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES include/gpcl.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS gpcl EXPORT gpcl-targets)
install(
  EXPORT gpcl-targets
  NAMESPACE gpcl::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gpcl)

install(FILES "LICENSE_1_0.txt" "README.md"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/gpcl)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE_1_0.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
include(CPack)
