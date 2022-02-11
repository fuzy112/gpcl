function(add_example_executable target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} PRIVATE gpcl::gpcl)

  if(BUILD_TESTING)
    add_test(NAME "example/${target}" COMMAND ${target} WORKING_DIRECTORY "${GPCL_BINARY_DIR}/bin")
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
