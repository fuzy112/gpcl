add_executable(memory_mapped_files memory_mapped_files.cpp)
target_link_libraries(memory_mapped_files gpcl::gpcl)

add_executable(shared_memory shared_memory.cpp)
target_link_libraries(shared_memory gpcl::gpcl)
