function(add_example_executable target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} PRIVATE gpcl::gpcl)
  set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                             "${GPCL_BINARY_DIR}/bin")
  if(BUILD_TESTING)
    add_test(NAME "example/${target}" COMMAND ${target})
  endif()

  if(GPCL_ENABLE_CLANG_TIDY)
    add_custom_command(
      TARGET ${target}
      PRE_BUILD
      COMMAND "${CLANG_TIDY_EXECUTABLE}" -p "${CMAKE_BINARY_DIR}" ${ARGN}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endfunction()

add_subdirectory(getopt)

if(NOT WIN32)
  add_subdirectory(shared_memory)
endif()

add_subdirectory(variant)
add_subdirectory(lexical_cast)
add_subdirectory(osyncstream)
add_subdirectory(json)

if(MSVC)
  add_subdirectory(auto_linking)
endif()

add_subdirectory(stacktrace)
add_subdirectory(process)
add_subdirectory(polymorphic_allocator)
add_subdirectory(exception)
add_subdirectory(sqlite)
add_subdirectory(dump_build_info)
add_subdirectory(service)
add_subdirectory(gc)
