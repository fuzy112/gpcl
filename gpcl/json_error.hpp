#ifndef GPCL_JSON_ERROR_HPP
#define GPCL_JSON_ERROR_HPP

#include <gpcl/error.hpp>

namespace gpcl {

class json_error;

/// JSON error code enum type.
/// `is_error_code_enum<json_errc>` is specialized to have a const static member
/// variable `bool value = true`, and `make_error_code(json_errc)` is overloaded
/// to create `error_code` from `json_errc`. Thus `json_errc` is implicitly
/// convertible to `error_code`.
enum class json_errc
{
  bad_json_cast = 1,
  type_mismatch,
  operation_not_supported,
  out_of_range,
  failed_to_parse,
};

/// Throws a json_error with a static message.
[[noreturn]] GPCL_DECL void throw_json_error(json_errc errc,
                                             const char *message, bool);

/// Returns a reference to the static error category object for errors reported
/// when processing JSON data.
/// The override version of `error_category::name()` returns `"json"`.
/// @sa json_errc defines error codes of this category.
GPCL_DECL error_category const &json_category() noexcept;

} // namespace gpcl

GPCL_DEFINE_MAKE_ERROR_CODE(gpcl::json_errc, gpcl::json_category())

GPCL_SPECIALIZE_IS_ERROR_CODE_ENUM(gpcl::json_errc, true)

#if defined GPCL_HEADER_ONLY
#  include <gpcl/impl/json_error.ipp>
#endif

#endif // GPCL_JSON_ERROR_HPP
