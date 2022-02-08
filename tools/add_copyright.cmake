file(GLOB_RECURSE SOURCES "${GPCL_SOURCE_DIR}/include/gpcl/*.?pp")

function(generate_copyright_notation_for_file filename OUT_VAR)
  get_filename_component(filename "${filename}" NAME)
  string(REGEX REPLACE "." "~" lines "${filename}")

  set(${OUT_VAR}
      "//
// ${filename}
// ${lines}
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
"
      PARENT_SCOPE)
endfunction()

set(changed_files "")

foreach(file ${SOURCES})
  file(READ "${file}" content)

  string(REGEX MATCH "tlsf\\..pp$" match_result "${file}")
  if(NOT "${match_result}" STREQUAL "")
    continue()
  endif()

  generate_copyright_notation_for_file("${file}" copyright)

  string(FIND "${content}" "Distributed under the Boost Software License" pos)
  if(pos STREQUAL "-1")
    set(new_content "${copyright}\n${content}")
    file(WRITE "${file}" "${new_content}")
    list(APPEND changed_files "${file}")
  endif()

endforeach(file ${SOURCES})

if(NOT changed_files STREQUAL "")
  list(JOIN changed_files "\n\t" changed_files_str)
  message("Changed files:\n\t${changed_files_str}")
endif()
