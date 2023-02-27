#
# CMakeLists.txt
# ~~~~~~~~~~~~~~
#
# Copyright (c) 2020-2023 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.16)

project(
  GPCL
  VERSION 0.9.0
  DESCRIPTION "General purpose C++ libraries"
  HOMEPAGE_URL "https://github.com/Chingyat/gpcl"
  LANGUAGES CXX C)

math(
  EXPR
  GPCL_VERSION_NUMBER
  "${GPCL_VERSION_MAJOR} * 10000 + ${GPCL_VERSION_MINOR} * 100 + ${GPCL_VERSION_PATCH}"
)

set(GPCL_CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${GPCL_CMAKE_MODULE_PATH}")

include(GNUInstallDirs)
include(CheckIPOSupported)
include(CTest)
include(FetchContent)
include(CMakePackageConfigHelpers)

option(GPCL_ENABLE_LTO "Enable link time optimisation" OFF)
option(GPCL_BUILD_TESTING "enable testing support" ON)
option(GPCL_BUILD_DOC "enable document generation" ON)
option(GPCL_FETCH_CPPREFERENCE "Download cppreference archive automatically"
       OFF)
option(GPCL_DOC_LINK_TO_CPPREFERENCE_ONLINE
       "link to external cppreference documentation" TRUE)
option(GPCL_ENABLE_LLVM "Link to LLVM if found" TRUE)
option(GPCL_LINK_TO_LLVM_SO "Link to libLLVM.so rather than LLVMSymbolize"
       TRUE)
option(GPCL_ENABLE_BFD "Link to BFD if found" TRUE)
option(GPCL_ENABLE_PCH "Enable precompiled headers" TRUE)
option(BUILD_SHARED_LIBS "Create shared libraies" TRUE)

option(GPCL_ENABLE_EXCEPTIONS "Enable exception support" TRUE)
option(GPCL_ENABLE_RTTI "Enable RTTI support" TRUE)

option(GPCL_ENABLE_CLANG_TIDY "Enable clang-tidy" FALSE)

option(GPCL_ENABLE_HEADER_TEST "Check whether public headers are self contained" FALSE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

check_ipo_supported(RESULT ipo_supported)
if(ipo_supported AND GPCL_ENABLE_LTO)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(NOT DEFINED BUILD_TESTING)
  set(BUILD_TESTING ${GPCL_BUILD_TESTING})
endif()

find_package(Threads)
find_package(LLVM CONFIG)
find_package(Doxygen)
find_package(BFD)
find_package(Backtrace)
find_package(SQLite3)
find_package(Git)

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
      CACHE BOOL "" FORCE)
elseif(BFD_FOUND AND "${GPCL_ENABLE_BFD}")
  target_link_libraries(gpcl PUBLIC BFD::BFD)
  target_compile_definitions(gpcl PUBLIC GPCL_BFD=1)
  set(GPCL_BFD
      TRUE
      CACHE BOOL "" FORCE)
endif()

if(Backtrace_FOUND)
  target_link_libraries(gpcl PUBLIC "${Backtrace_LIBRARIES}")
  target_include_directories(gpcl PUBLIC "${Backtrace_INCLUDE_DIRS}")
  target_compile_definitions(gpcl
                             PUBLIC GPCL_BACKTRACE_HEADER="${Backtrace_HEADER}")
endif()

if(SQLite3_FOUND)
  target_link_libraries(gpcl PUBLIC SQLite::SQLite3)
  target_compile_definitions(gpcl PUBLIC GPCL_SQLITE=1)
  set(GPCL_SQLITE
      TRUE
      CACHE BOOL "" FORCE)
endif()

if(GIT_FOUND)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse "--verify" HEAD
    OUTPUT_VARIABLE GPCL_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(GPCL_GIT_HASH)
    target_compile_definitions(gpcl PRIVATE GPCL_GIT_HASH=\"${GPCL_GIT_HASH}\")
  endif()

  execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe
    OUTPUT_VARIABLE GPCL_GIT_DESCRIPTION
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  if(GPCL_GIT_DESCRIPTION)
    target_compile_definitions(
      gpcl PRIVATE GPCL_GIT_DESCRIPTION=\"${GPCL_GIT_DESCRIPTION}\")
  endif()
endif()

if(GPCL_GIT_HASH)
  set(GPCL_IS_GIT_REPO TRUE)
else()
  set(GPCL_IS_GIT_REPO FALSE)
endif()

if(GPCL_IS_GIT_REPO)
  message(INFO "Installing pre-commit hook")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/tools/pre-commit"
    DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/.git/hooks/")
endif()

if(UNIX AND NOT APPLE)
  target_link_libraries(gpcl PUBLIC rt)
endif()

if(WIN32)
  target_link_libraries(gpcl PUBLIC dbghelp synchronization)
endif()

if(MINGW)
  target_compile_options(gpcl INTERFACE "-Wa,-mbig-obj")
  target_link_libraries(gpcl PUBLIC)
endif()

if(NOT GPCL_ENABLE_EXCEPTIONS)
  target_compile_definitions(gpcl PUBLIC GPCL_CONFIG_NO_EXCEPTIONS=1)
else()
  if(MSVC)
    target_compile_options(gpcl PUBLIC /EHsc)
  endif()
endif()

if(NOT GPCL_ENABLE_RTTI)
  target_compile_definitions(gpcl PUBLIC GPCL_CONFIG_NO_RTTI=1)
endif()

if(CMAKE_DL_LIBS)
  target_link_libraries(gpcl PUBLIC ${CMAKE_DL_LIBS})
endif()

mark_as_advanced(GPCL_BFD GPCL_LLVM GPCL_SQLITE)

target_compile_features(gpcl PUBLIC cxx_std_17)

get_target_property(library_type gpcl TYPE)

if("${library_type}" STREQUAL "SHARED_LIBRARY")
  target_compile_definitions(gpcl PUBLIC GPCL_DYN_LINK=1)
endif()

if(EMSCRIPTEN)
  target_compile_options(gpcl PUBLIC "-pthread")
  target_link_options(gpcl PUBLIC "-pthread")
endif()

set_target_properties(
  gpcl
  PROPERTIES SOVERSION "${GPCL_VERSION_NUMBER}"
             VERSION ${GPCL_VERSION}
             DEFINE_SYMBOL GPCL_SOURCE
             LIBRARY_OUTPUT_DIRECTORY "${GPCL_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${GPCL_BINARY_DIR}/bin"
             ARCHIVE_OUTPUT_DIRECTORY "${GPCL_BINARY_DIR}/lib")

set(GPCL_CPPREFERENCE_DIR
    ""
    CACHE PATH "Path to cppreference directory")

FetchContent_Declare(
  cppreference
  URL https://upload.cppreference.com/mwiki/images/1/16/html_book_20190607.tar.xz
  URL_HASH
    SHA256=8f97b2baa749c748a2e022d785f1a2e95aa851a3075987dfcf38baf65e0e486d)

if(Doxygen_FOUND AND GPCL_BUILD_DOC)

  if(GPCL_FETCH_CPPREFERENCE)
    FetchContent_MakeAvailable(cppreference)

    if(NOT GPCL_CPPREFERENCE_DIR)
      set(GPCL_CPPREFERENCE_DIR
          "${cppreference_SOURCE_DIR}"
          CACHE PATH "Path to cppreference directory" FORCE)
    endif()
  endif()

  if(GPCL_CPPREFERENCE_DIR)
    if(GPCL_DOC_LINK_TO_CPPREFERENCE_ONLINE)
      list(
        APPEND
        DOXYGEN_TAGFILES
        "${GPCL_CPPREFERENCE_DIR}/cppreference-doxygen-web.tag.xml=http://en.cppreference.com/w/"
      )
    else()
      list(
        APPEND
        DOXYGEN_TAGFILES
        "${GPCL_CPPREFERENCE_DIR}/cppreference-doxygen-local.tag.xml=${GPCL_CPPREFERENCE_DIR}/reference/en/"
      )
    endif()
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

configure_file(cmake/set_module_path.cmake.in
               "${GPCL_BINARY_DIR}/lib/cmake/gpcl/set_module_path.cmake")

export(
  TARGETS gpcl
  NAMESPACE gpcl::
  FILE "${GPCL_BINARY_DIR}/lib/cmake/gpcl/gpcl-targets.cmake")

configure_package_config_file(
  cmake/gpcl-config.cmake.in
  "${GPCL_BINARY_DIR}/lib/cmake/gpcl/gpcl-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gpcl"
  PATH_VARS CMAKE_INSTALL_LIBDIR GPCL_CMAKE_MODULE_PATH)

write_basic_package_version_file(
  "${GPCL_BINARY_DIR}/lib/cmake/gpcl/gpcl-config-version.cmake"
  COMPATIBILITY SameMinorVersion)

install(FILES "${GPCL_BINARY_DIR}/lib/cmake/gpcl/gpcl-config.cmake"
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

set(CPACK_PACKAGE_NAME gpcl)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE_1_0.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
include(CPack)

if(GPCL_ENABLE_CLANG_TIDY)
  find_package(ClangTidy REQUIRED)
  add_custom_target(
    clang-tidy ALL
    COMMAND
      ${CLANG_TIDY_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/include/gpcl/*.hpp"
      "-p" "${CMAKE_BINARY_DIR}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(GPCL_ENABLE_HEADER_TEST)
  include(tools/generate_header_tests.cmake)
endif()
