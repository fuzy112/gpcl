add_library(separate src.cpp)
add_library(gpcl::separate ALIAS separate)

target_link_libraries(separate PUBLIC gpcl::gpcl)

if (UNIX)
	target_link_libraries(separate
		PUBLIC
		pthread
		rt
		)

	# target_compile_options(gpcl-separate INTERFACE "-fno-exceptions")

endif ()

target_compile_definitions(separate PUBLIC GPCL_SEPARATE_COMPILATION=1)

set_target_properties(separate
	PROPERTIES
	OUTPUT_NAME gpcl
)
