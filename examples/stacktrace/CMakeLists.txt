add_example_executable(stacktrace main.cpp)

if (WIN32)
    target_link_libraries(stacktrace PRIVATE DbgHelp)
endif ()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_options(stacktrace PRIVATE "-rdynamic")
endif()

set_target_properties(stacktrace
    PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)