find_program(
  CLANG_TIDY_EXECUTABLE NAMES clang-tidy-13 clang-tidy-12 clang-tidy-11
                             clang-tidy-10 clang-tidy-9 clang-tidy)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangTidy DEFAULT_MSG CLANG_TIDY_EXECUTABLE)
