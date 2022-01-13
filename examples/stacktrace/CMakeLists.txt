add_example_executable(stacktrace main.cpp)

if (WIN32)
    target_link_libraries(stacktrace PRIVATE DbgHelp)
endif ()
