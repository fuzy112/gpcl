add_executable(service main.cpp)
target_link_libraries(service PRIVATE gpcl::gpcl)
set_target_properties(service
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${GPCL_BINARY_DIR}/bin")
