find_library(BFD_LIBRARY NAMES libbfd.so libbfd.a bfd)
find_path(BFD_INCLUDE_DIR NAMES bfd.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  BFD
  DEFAULT_MSG
  BFD_LIBRARY
  BFD_INCLUDE_DIR
)

if (BFD_FOUND AND NOT TARGET bfd::bfd)
  add_library(bfd::bfd UNKNOWN IMPORTED)
  set_target_properties(bfd::bfd PROPERTIES IMPORED_LOCATION "${BFD_LIBRARY}"
                                            INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}"
                                            INTERFACE_INCLUDE_DIRECTORIES "${BFD_INCLUDE_DIR}")
endif ()

set(BFD_LIBRARIES "${BFD_LIBRARY}")
set(BFD_INCLUDE_DIRS "${BFD_INCLUDE_DIR}")

mark_as_advanced(BFD_LIBRARY BFD_INCLUDE_DIR)
