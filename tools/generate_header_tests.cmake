add_library(header_tests)
target_link_libraries(header_tests PRIVATE gpcl::gpcl)
file(GLOB files "${GPCL_SOURCE_DIR}/include/gpcl/*.hpp"
  "${GPCL_SOURCE_DIR}/include/gpcl/pmr/*.hpp")

foreach(file ${files})
  get_filename_component(filename ${file} NAME)

  if(NOT filename STREQUAL "ext.hpp")
    set(HEADER_FILE ${file})
    configure_file(${CMAKE_CURRENT_LIST_DIR}/header_test.cpp.in "header_tests/${filename}.cpp")
    target_sources(header_tests PRIVATE "header_tests/${filename}.cpp")
  endif()
endforeach(file ${files})
