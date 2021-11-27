enable_testing()
include(CTest)

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
)
add_executable(tests ${TESTS_SRCS} 	include_src.cpp)
target_link_libraries(tests PRIVATE gpcl::gpcl)

add_executable(tests-separate ${TESTS_SRCS})
target_link_libraries(tests-separate PRIVATE gpcl::separate)
