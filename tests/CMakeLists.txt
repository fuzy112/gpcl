set(TESTS_SRCS
    any_test.cpp
    atomic_test.cpp
    bind_front_test.cpp
    buffers_test.cpp
    condition_variable_test.cpp
    dynarray_test.cpp
    exception.cpp
    expected_test.cpp
    file_test.cpp
    function_test.cpp
    futex_atomic_wait_test.cpp
    future_test.cpp
    invoke_test.cpp
    lexical_cast_test.cpp
    list_test.cpp
    lock_file_test.cpp
    monotonic_buffer_resource_test.cpp
    mutex_test.cpp
    once_flag_test.cpp
    pid_file.cpp
    scope_test.cpp
    scoped_array_test.cpp
    shared_ptr_test.cpp
    simple_segregated_storage_test.cpp
    syncbuf_test.cpp
    thread_test.cpp
    time_test.cpp
    tuple_for_each_test.cpp
    unique_ptr_test.cpp
    vector_test.cpp
    out_ptr_test.cpp
    string_utils_test.cpp
    symbol_location_test.cpp
    pool_test.cpp
    compressed_pointer_test.cpp
    clone_ptr_test.cpp
    )

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
  set_target_properties(
    "${name}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${GPCL_BINARY_DIR}/bin"
                         EXPORT_COMPILE_COMMANDS OFF)
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(doctest_main OBJECT main.cpp)
target_link_libraries(doctest_main PUBLIC gpcl::gpcl)
add_executable(tests ${TESTS_SRCS})
target_link_libraries(tests PRIVATE doctest_main ${TEST_FIXTURES})
if(GPCL_ENABLE_PCH)
  target_precompile_headers(tests PRIVATE <gpcl.hpp> "doctest.h")
endif()
set_target_properties(
  tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${GPCL_BINARY_DIR}/bin"
                   EXPORT_COMPILE_COMMANDS OFF)

include(doctest.cmake)

if(NOT CMAKE_CROSSCOMPILING)
  doctest_discover_tests(tests)
else()
  add_test(NAME tests COMMAND tests)
endif()

add_executable(meta-test meta_test.cpp)
target_link_libraries(meta-test PRIVATE gpcl::gpcl)
add_test(NAME meta-test COMMAND meta-test)
set_target_properties(meta-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                           "${GPCL_BINARY_DIR}/bin")
