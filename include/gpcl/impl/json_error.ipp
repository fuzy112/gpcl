//
// json_error.ipp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_JSON_ERROR_IPP
#define GPCL_IMPL_JSON_ERROR_IPP

#include <gpcl/json_error.hpp>

namespace gpcl {

error_category const &json_category() noexcept
{
  class json_category_impl final : public error_category
  {
    struct error_info
    {
      const char *message;
      errc condition = errc();
    };

    static inline constexpr error_info info(json_errc code) noexcept
    {
      switch (code)
      {
      case json_errc::bad_json_cast:
        return {
            "bad json cast",
        };

      case json_errc::type_mismatch:
        return {
            "type mismatch",
        };

      case json_errc::operation_not_supported:
        return {
            "operation not supported",
            errc::operation_not_supported,
        };

      case json_errc::out_of_range:
        return {
            "out of range",
            errc::invalid_argument,
        };

      case json_errc::failed_to_parse:
        return {
            "failed to parse",
        };

      default:
        GPCL_UNREACHABLE("invalid error code");
      }
    }

  public:
    json_category_impl() {}

    const char *name() const noexcept { return "json"; }

    std::string message(int code) const noexcept
    {
      return std::string(info(json_errc(code)).message);
    }

    error_condition default_error_condition(int code) const noexcept
    {
      if (auto condition = info(json_errc(code)).condition; condition != errc())
      {
        return condition;
      }
      return error_category::default_error_condition(code);
    }
  };

  const static json_category_impl instance = {};
  return instance;
}

void throw_json_error(json_errc errc, const char *message, bool)
{
  GPCL_THROW(json_error(errc, message, json_error::const_string_tag{}));
}

} // namespace gpcl

#endif // GPCL_IMPL_JSON_ERROR_IPP
