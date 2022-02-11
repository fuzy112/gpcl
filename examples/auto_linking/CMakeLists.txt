add_executable(auto_linking main.cpp)
target_include_directories(auto_linking PRIVATE ${GPCL_SOURCE_DIR}/include)
target_link_directories(auto_linking PRIVATE ${GPCL_BINARY_DIR}/lib)
target_compile_features(auto_linking PRIVATE cxx_std_17)

# ensure this is built after gpcl
add_dependencies(auto_linking gpcl::gpcl)
