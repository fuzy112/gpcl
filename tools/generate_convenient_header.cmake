set(INCLUDES "")

file(GLOB files "${GPCL_SOURCE_DIR}/gpcl/*.hpp")
foreach (file ${files})
    get_filename_component(filename ${file} NAME)
    if (NOT filename STREQUAL "ext.hpp")
        string(APPEND INCLUDES "#include <gpcl/${filename}>\n")
    endif ()
endforeach (file ${files})

configure_file("${CMAKE_CURRENT_LIST_DIR}/gpcl.hpp.in" "${GPCL_SOURCE_DIR}/gpcl.hpp")
