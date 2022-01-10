add_library(separate src.cpp)
add_library(gpcl::separate ALIAS separate)

target_link_libraries(separate PUBLIC gpcl::gpcl)

target_compile_definitions(separate PUBLIC GPCL_SEPARATE_COMPILATION=1)

get_target_property(separate_library_type separate TYPE)

if ("${separate_library_type}" STREQUAL "SHARED_LIBRARY")
	target_compile_definitions(separate PUBLIC GPCL_DYN_LINK=1)
endif ()

set_target_properties(separate
	PROPERTIES
	OUTPUT_NAME gpcl
	SOVERSION 500
	VERSION 0.5.0
)
