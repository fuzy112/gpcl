find_library(IBERTY_LIBRARY NAMES libiberty.a iberty)
find_path(IBERTY_INCLUDE_DIR NAMES libiberty.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Iberty DEFAULT_MSG Iberty_LIBRARY Iberty_INCLUDE_DIR)

if(Iberty_FOUND AND NOT TARGET Iberty::Iberty)
  add_library(Iberty::Iberty UNKNOWN IMPORTED)
  set_target_properties(
    Iberty::Iberty
    PROPERTIES IMPORTED_LOCATION "${Iberty_LIBRARY}"
               INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}"
               INTERFACE_INCLUDE_DIRECTORIES "${Iberty_INCLUDE_DIR}")
endif()

set(Iberty_LIBRARIES "${Iberty_LIBRARY}")
set(Iberty_INCLUDE_DIRS "${Iberty_INCLUDE_DIR}")

mark_as_advanced(Iberty_LIBRARY Iberty_INCLUDE_DIR)
