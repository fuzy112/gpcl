//
// getopt.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_GETOPT_HPP
#define GPCL_GETOPT_HPP

#include <gpcl/span.hpp>

#include <iosfwd>

namespace gpcl {

/// @defgroup command_line_parser Command line parser
/// @{

/// Option argument type.
enum class getopt_option_type
{
  /// The option has no argument.
  no_arg,

  /// The option has a required argument.
  required,

  /// The option has an optional argument.
  optional,

  /// The value is set to flag.
  flag_set,

  /// The value is AND'ed to the flag.
  flag_and,

  /// The value is OR'ed to the flag.
  flag_or,
};

constexpr getopt_option_type getopt_no_arg = getopt_option_type::no_arg;
constexpr getopt_option_type getopt_required = getopt_option_type::required;
constexpr getopt_option_type getopt_optional = getopt_option_type::optional;
constexpr getopt_option_type getopt_flag_set = getopt_option_type::flag_set;
constexpr getopt_option_type getopt_flag_and = getopt_option_type::flag_and;
constexpr getopt_option_type getopt_flag_or = getopt_option_type::flag_or;

/// Description of an option.
struct getopt_option
{
  /// Option name.
  const char *name;

  /// Short option name.
  char name_short;

  /// Option argument type.
  getopt_option_type type;

  /// Pointer to a flag effected by this option.
  /// @see getopt_option_type.
  int *flag;

  /// Option value. 
  /// @see getopt_option_type
  /// @see flag
  int value;

  /// Description text of the option.
  const char *desc;

  /// Description text of the option argument.
  const char *value_desc;
};

/// Command line parser.
/**
 * @par Example
 * @code
#include <gpcl/getopt.hpp>

inline int verbosity = 0;

constexpr const gpcl::getopt_option options[] = {
    {"help", 'h', gpcl::getopt_no_arg, nullptr, 'h',
     "Print this help message."},
    {"verbose", 'v', gpcl::getopt_optional, nullptr, 'v',
     "Set the verbosity of the program.", "0-5"},
    {},
};

int main(int argc, char **argv)
{
  gpcl::getopt getopt(argc, argv, options);

  int c;
  while ((c = getopt()) != -1)
  {
    switch (c)
    {
    case 'h':
      std::cout << getopt << std::endl;
      break;

    case 'v':
      if (getopt.optarg())
        verbosity = atoi(getopt.optarg());
      else
        verbosity = 5;
      break;

    default:
      break;
    }
  }

  std::cout << "Verbosity: " << verbosity << "\n";
}

 * @endcode
 */
class getopt
{
  // arguments to parse.
  gpcl::span<const char *const> argv_;

  // option definitions.
  gpcl::span<const getopt_option> opts_;

  // index of arguments in argv.
  gpcl::span<const char *const>::size_type optind_;

  // position of short options in a string.
  int optpos_;

  // argument of an option.
  const char *optarg_;

  // current option description.
  const getopt_option *opt_;

public:
  /// Construct a getopt.
  GPCL_DECL getopt(int argc, const char *const argv[],
                   gpcl::span<const getopt_option> opts);

  /// Get next option.
  GPCL_DECL int operator()();

  /// Get the current option argument.
  const char *optarg() const noexcept { return optarg_; }

  /// Get the current option definition.
  const getopt_option *option() const noexcept { return opt_; }

  /// Get all known options.
  span<const getopt_option> all_options() const noexcept { return opts_; }

  /// Get all commandline arguments.
  span<const char *const> argv() const noexcept { return argv_; }
};

/// Print options to a stream.
/// @relates gpcl::getopt
GPCL_DECL std::ostream &operator<<(std::ostream &out, const getopt &parser);

/// @}

} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/impl/getopt.ipp>
#endif

#endif // GPCL_GETOPT_HPP
