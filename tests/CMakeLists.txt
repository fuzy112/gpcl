set (TESTS_SRCS
    main.cpp
    expected_test.cpp
    monotonic_buffer_resource_test.cpp
	time_test.cpp
	file_test.cpp
    lock_file_test.cpp
	future_test.cpp
	unique_resource_test.cpp
	pid_file.cpp
	mutex_test.cpp
	once_flag_test.cpp
	futex_atomic_wait_test.cpp
	any_test.cpp
	function_test.cpp
	unique_ptr_test.cpp
	vector_test.cpp
	shared_ptr_test.cpp
	condition_variable_test.cpp
	simple_segregated_storage_test.cpp
	syncbuf_test.cpp
	buffers_test.cpp
)

set (TEST_FIXTURES
	syncbuf)


set (TEST_FIXTURES_HEADER_ONLY)
set (TEST_FIXTURES_SEPARATE)

include(GenerateExportHeader)

foreach (name ${TEST_FIXTURES})
	aux_source_directory("${name}" fixture_sources)

	add_library("${name}-header_only" STATIC ${fixture_sources})
	target_compile_definitions("${name}-header_only" PUBLIC "-D$<UPPER_CASE:${name}>_STATIC_DEFINE")
	target_link_libraries("${name}-header_only" PRIVATE gpcl::gpcl)
	list (APPEND TEST_FIXTURES_HEADER_ONLY "${name}-header_only")

	add_library("${name}-separate" ${fixture_sources})
	GENERATE_EXPORT_HEADER("${name}-separate" BASE_NAME "${name}")
	target_link_libraries("${name}-separate" PRIVATE gpcl::separate)
	list (APPEND TEST_FIXTURES_SEPARATE "${name}-separate")
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(tests ${TESTS_SRCS} include_src.cpp)
target_link_libraries(tests PRIVATE gpcl::gpcl ${TEST_FIXTURES_HEADER_ONLY})

add_executable(tests-separate ${TESTS_SRCS})
target_link_libraries(tests-separate PRIVATE gpcl::separate ${TEST_FIXTURES_SEPARATE})

include(doctest.cmake)

doctest_discover_tests(tests TEST_PREFIX "*header only*")
doctest_discover_tests(tests-separate TEST_PREFIX "*separate*")

add_executable(meta-test meta_test.cpp)
target_link_libraries(meta-test PRIVATE gpcl::gpcl)
add_test(NAME meta-test COMMAND meta-test)
