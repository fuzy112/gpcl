find_library(BFD_LIBRARY NAMES libbfd.so bfd)
find_path(BFD_INCLUDE_DIR NAMES bfd.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD DEFAULT_MSG BFD_LIBRARY BFD_INCLUDE_DIR)

find_package(Iberty)
find_package(Intl)
find_package(ZLIB)

if(BFD_FOUND AND NOT TARGET BFD::BFD)
  add_library(BFD::BFD UNKNOWN IMPORTED)
  set_target_properties(
    BFD::BFD
    PROPERTIES IMPORTED_LOCATION "${BFD_LIBRARY}"
               INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}"
               INTERFACE_INCLUDE_DIRECTORIES "${BFD_INCLUDE_DIR}")

  if(Iberty_FOUND)
    target_link_libraries(BFD::BFD INTERFACE Iberty::Iberty)
  endif()
  if(Intl_FOUND)
    target_link_libraries(BFD::BFD INTERFACE "${Intl_LIBRARIES}")
  endif()
  if(ZLIB_FOUND)
    target_link_libraries(BFD::BFD INTERFACE ZLIB::ZLIB)
  endif()
endif()

set(BFD_LIBRARIES "${BFD_LIBRARY}" "${Iberty_LIBRARIES}" "${Intl_LIBRARIES}" "${ZLIB_LIBRARIES}")
set(BFD_INCLUDE_DIRS "${BFD_INCLUDE_DIR}" "${Iberty_INCLUDE_DIRS}" "${Intl_INCLUDE_DIRS}" "${ZLIB_INCLUDE_DIRS}")

mark_as_advanced(BFD_LIBRARY BFD_INCLUDE_DIR)
