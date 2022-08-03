add_executable(auto_linking main.cpp)
target_include_directories(auto_linking PRIVATE ${GPCL_SOURCE_DIR}/include)
target_link_directories(auto_linking PRIVATE ${GPCL_BINARY_DIR}/lib ${GPCL_BINARY_DIR}/lib/$<CONFIG>)
target_compile_features(auto_linking PRIVATE cxx_std_17)
if(GPCL_ENABLE_EXCEPTIONS)
  target_compile_options(auto_linking PRIVATE "/EHsc")
endif()

# ensure this is built after gpcl
add_dependencies(auto_linking gpcl::gpcl)
