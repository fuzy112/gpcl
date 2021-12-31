add_subdirectory(getopt)

if (NOT WIN32)
    add_subdirectory(shared_memory)
endif ()

add_subdirectory(variant)
add_subdirectory(lexical_cast)
add_subdirectory(osyncstream)
