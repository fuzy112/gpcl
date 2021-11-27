#
# CMakeLists.txt
# ~~~~~~~~~~~~~~
#
# Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.10)

project(GPCL CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

if (MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${LIB_SUFFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${LIB_SUFFIX})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost
    COMPONENTS system
    )

set(GPCL_HEADERS)

add_library(gpcl INTERFACE)
add_library(gpcl::gpcl ALIAS gpcl)
target_include_directories(gpcl
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)
if (UNIX)
	target_link_libraries(gpcl
		INTERFACE
		pthread
		)

	# target_compile_options(gpcl INTERFACE "-fno-exceptions")
endif ()

add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(tools)

install(DIRECTORY gpcl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES gpcl.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS gpcl separate EXPORT gpcl-targets)
install(EXPORT gpcl-targets NAMESPACE gpcl::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gpcl)

include(CPack)
