add_example_executable(rbtree_test rbtree_test.cpp)

if (GPCL_WITH_LIBEDITLINE)
	target_link_libraries (rbtree_test PRIVATE -leditline)
	target_compile_definitions (rbtree_test PRIVATE WITH_LIBEDITLINE)
endif()