//
// json_error.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_JSON_ERROR_HPP
#define GPCL_JSON_ERROR_HPP

#include <gpcl/error.hpp>
#include <gpcl/exception.hpp>

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

using json_type_errinfo =
    error_info<struct json_type_err_tag, std::pair<std::string, std::string>>;

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

namespace gpcl {

/// The class json_error defines an exception object thrown when processing JSON
/// data.
class json_error : virtual public system_error, virtual public exception
{
  const char *const_message_ = nullptr;

public:
  explicit json_error(json_errc errc)
      : system_error(static_cast<int>(errc), json_category()),
        gpcl::exception()
  {
  }

  explicit json_error(json_errc errc, const std::string &what)
      : system_error(static_cast<int>(errc), json_category(), what),
        gpcl::exception()
  {
  }

  explicit json_error(json_errc errc, const char *what)
      : system_error(static_cast<int>(errc), json_category(), what),
        gpcl::exception()
  {
  }

  struct const_string_tag
  {
  };

  json_error(json_errc errc, const char *what, const_string_tag)
      : system_error(static_cast<int>(errc), json_category()),
        const_message_(what)
  {
  }

  const char *what() const noexcept
  {
    if (const_message_)
      return const_message_;

    return system_error::what();
  }
};
} // namespace gpcl

#endif // GPCL_JSON_ERROR_HPP
