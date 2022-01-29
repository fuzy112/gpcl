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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

option (GPCL_ENABLE_LTO "Enable link time optimisation" OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if (ipo_supported AND GPCL_ENABLE_LTO)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${LIB_SUFFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${LIB_SUFFIX})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(GPCL_BUILD_TESTING "enable testing support" ON)
if (NOT DEFINED BUILD_TESTING)
   set (BUILD_TESTING ${GPCL_BUILD_TESTING})
endif()

option(GPCL_BUILD_DOC "enable document generation" ON)

include(CTest)

find_package(Boost
    COMPONENTS system
    )

file(GLOB_RECURSE GPCL_HEADERS CONFIGURE_DEPENDS
	gpcl/*.hpp
	gpcl/*.ipp
)

set(GPCL_SOURCES lib/src.cpp)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(gpcl ${GPCL_HEADERS} ${GPCL_SOURCES})
add_library(gpcl::gpcl ALIAS gpcl)

target_compile_features(gpcl PUBLIC cxx_std_17)

find_package(Threads QUIET)
if (Threads_FOUND)
	target_link_libraries (gpcl PUBLIC Threads::Threads)
endif()

target_include_directories(gpcl
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)
if (UNIX)
	target_link_libraries(gpcl
		PUBLIC
		rt
		execinfo
		${CMAKE_DL_LIBS}
		)
endif ()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_options(gpcl PUBLIC "-rdynamic")
    target_link_libraries(gpcl
	    PUBLIC
	    bfd
	    )
endif()

if (CYGWIN)
	target_link_libraries(gpcl
		PUBLIC
		bfd
		iberty
		intl
		z
		${CMAKE_DL_LIBS}
		)
endif ()

if (WIN32)
	target_link_libraries(gpcl
		PUBLIC
		DbgHelp
		)
endif()

if (MINGW)
    target_compile_options(gpcl INTERFACE "-Wa,-mbig-obj")
	target_link_libraries(gpcl
		PUBLIC
	)
endif ()

if (GPCL_ENABLE_PCH)
	target_precompile_headers(
		gpcl
		INTERFACE
		$<BUILD_INTERFACE:gpcl.hpp>
	)
endif ()

get_target_property(library_type gpcl TYPE)

if ("${library_type}" STREQUAL "SHARED_LIBRARY")
	target_compile_definitions(gpcl PUBLIC GPCL_DYN_LINK=1)
endif ()

set_target_properties(gpcl
	PROPERTIES
	SOVERSION 600
	VERSION 0.6.0
	DEFINE_SYMBOL GPCL_SOURCE
)

include (FetchContent)
FetchContent_Declare(
	cppreference
	URL https://upload.cppreference.com/mwiki/images/1/16/html_book_20190607.tar.xz
	URL_HASH SHA256=8f97b2baa749c748a2e022d785f1a2e95aa851a3075987dfcf38baf65e0e486d
)

find_package(Doxygen)

option (GPCL_DOC_LINK_TO_CPPREFERENCE_ONLINE "link to external cppreference documentation" TRUE)

if (Doxygen_FOUND AND GPCL_BUILD_DOC)

	FetchContent_MakeAvailable(cppreference)
	if (GPCL_DOC_LINK_TO_CPPREFERENCE_ONLINE)
		list (APPEND DOXYGEN_TAGFILES "${cppreference_SOURCE_DIR}/cppreference-doxygen-web.tag.xml=http://en.cppreference.com/w/")
	else ()
		list (APPEND DOXYGEN_TAGFILES "${cppreference_SOURCE_DIR}/cppreference-doxygen-local.tag.xml=${cppreference_SOURCE_DIR}/reference/en/")
	endif ()
	set (DOXYGEN_PREDEFINED GPCL_DOXYGEN)

	set (DOXYGEN_JAVADOC_AUTOBRIEF YES)
	set (DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
	SET (DOXYGEN_EXTRACT_PRIVATE YES)
	# set (DOXYGEN_EXCLUDE_SYMBOLS "*::detail::*;*_detail::*")
	set (DOXYGEN_STRIP_FROM_INC_PATH ".")
    doxygen_add_docs(doc README.md lib gpcl)

	find_package(Python COMPONENTS Interpreter QUIET)
	if (Python_FOUND)
		add_custom_target(serv_doc
			${Python_EXECUTABLE} -m http.server 3333
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
			USES_TERMINAL)
	endif ()
endif ()

add_subdirectory(tools)

add_subdirectory(examples)

if (BUILD_TESTING)
	add_subdirectory(tests)
endif ()

configure_file(gpcl-config.cmake.in gpcl-config.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gpcl-config.cmake" 
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gpcl)

install(DIRECTORY gpcl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES gpcl.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS gpcl EXPORT gpcl-targets)
install(EXPORT gpcl-targets NAMESPACE gpcl::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gpcl)

include(CPack)

