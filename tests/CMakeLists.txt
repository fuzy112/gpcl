set(TESTS_SRCS
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
    bind_front_test.cpp
    thread_test.cpp
    scope_test.cpp
    exception.cpp
    scoped_array_test.cpp)

set(TEST_FIXTURES syncbuf)

include(GenerateExportHeader)

foreach(name ${TEST_FIXTURES})
  aux_source_directory("${name}" fixture_sources)
  add_library("${name}" ${fixture_sources})
  generate_export_header("${name}")
  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions("${name}"
                               PUBLIC "-D$<UPPER_CASE:${name}>_STATIC_DEFINE")
  endif()
  target_link_libraries("${name}" PUBLIC gpcl::gpcl)
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(tests ${TESTS_SRCS} include_src.cpp)
target_link_libraries(tests PRIVATE gpcl::gpcl ${TEST_FIXTURES})

include(doctest.cmake)

doctest_discover_tests(tests)

add_executable(meta-test meta_test.cpp)
target_link_libraries(meta-test PRIVATE gpcl::gpcl)
add_test(NAME meta-test COMMAND meta-test)
