add_custom_target(
  check-copyright
  ${CMAKE_COMMAND} "-DGPCL_SOURCE_DIR=${GPCL_SOURCE_DIR}" -P add_copyright.cmake
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(
  update-gpcl.hpp
  ${CMAKE_COMMAND} "-DGPCL_SOURCE_DIR=${GPCL_SOURCE_DIR}" -P
  generate_convenient_header.cmake
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
