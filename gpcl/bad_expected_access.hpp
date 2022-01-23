//
// bad_expected_access.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BAD_EXPECTED_ACCESS_HPP
#define GPCL_BAD_EXPECTED_ACCESS_HPP

#include <gpcl/detail/error_formatter.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/expected_fwd.hpp>

#include <exception>

namespace gpcl {

template <>
class bad_expected_access<void> : virtual public std::exception
{
public:
  explicit bad_expected_access() = default;
};

/// Exception thrown when trying to access the value from an expected that
/// contains an error.
template <typename E>
class bad_expected_access : public bad_expected_access<void>
{
public:
  inline explicit bad_expected_access(E e) : val(detail::move(e))
  {
    what_ = "bad_exception_access: ";
    detail::format_error_to(std::back_inserter(what_), val);
  }

  /// Returns a string that describes the error.
  [[nodiscard]] czstring<> what() const noexcept override
  {
    return what_.c_str();
  }

  E &error() &noexcept { return val; }
  const E &error() const &noexcept { return val; }
  E &&error() &&noexcept { return detail::move(val); }
  const E &&error() const &&noexcept { return detail::move(val); }

private:
  E val;
  std::string what_;
};

} // namespace gpcl

#endif