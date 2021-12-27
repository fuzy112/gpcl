//
// getopt.ipp
// ~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_GETOPT_IPP
#define GPCL_GETOPT_IPP

#include <gpcl/getopt.hpp>

#include <iomanip>
#include <sstream>
#include <string_view>

namespace gpcl {

getopt::getopt(int argc, const char *const argv[],
               span<const getopt_option> opts)
    : argv_(argv, argc),
      opts_(opts),
      optind_(),
      optpos_(),
      optarg_()
{
}

int getopt::operator()()
{
  optarg_ = nullptr;
  opt_ = nullptr;

  if (optind_ == 0)
    optind_ = 1;

  if (argv_.size() <= optind_ || argv_[optind_] == nullptr)
    return -1;

  if (argv_[optind_][0] != '-')
    return -1;

  if (argv_[optind_][1] == '-' && argv_[optind_][2] == '\0')
  {
    ++optind_;
    return -1;
  }

  if (argv_[optind_][1] == '-') // long option
  {
    std::string_view arg(&argv_[optind_][2]);
    auto pos = arg.find('=');
    if (pos != std::string_view::npos)
      arg = arg.substr(0, pos);
    for (auto &o : opts_)
    {
      if (!!o.name && arg == o.name)
      {
        opt_ = &o;

        if (pos != std::string_view::npos)
        {
          optpos_ = 2 + pos + 1;
        }
        else
        {
          optind_++;
          optpos_ = 0;
        }
        goto handle_option;
      }
    }

    return '?';
  }

  if (optpos_ == 0)
    optpos_ = 1;

  {
    char c = argv_[optind_][optpos_++];
    if (argv_[optind_][optpos_] == '\0')
    {
      ++optind_;
      optpos_ = 0;
    }

    for (auto &o : opts_)
    {
      if (c == o.name_short)
      {
        opt_ = &o;
        goto handle_option;
      }
    }
    return '?';
  }

handle_option:
  if (opt_->type == getopt_option_type::no_arg)
    optarg_ = nullptr;
  if (opt_->type == getopt_option_type::required ||
      (opt_->type == getopt_option_type::optional && optind_ < argv_.size() &&
       argv_[optind_][optpos_] != '-'))
  {
    optarg_ = &argv_[optind_++][optpos_];
    optpos_ = 0;
  }

  if (opt_->flag)
  {
    if (opt_->type == getopt_option_type::flag_set)
      *opt_->flag = opt_->value;
    else if (opt_->type == getopt_option_type::flag_and)
      *opt_->flag &= opt_->value;
    else if (opt_->type == getopt_option_type::flag_or)
      *opt_->flag |= opt_->value;
    else
      return '?';
    return 0;
  }

  return opt_->value;
}

std::ostream &operator<<(std::ostream &out, const getopt &parser)
{
  out << "Options:\n";
  for (auto &opt : parser.all_options())
  {
    if (!opt.name && !opt.name_short)
      break;
    out << "    ";
    if (opt.name_short)
    {
      out << '-' << opt.name_short;
    }

    std::stringstream long_opt;
    if (opt.name)
    {
      if (opt.name_short)
      {
        out << ",";
      }

      long_opt << "\t--" << opt.name;
    }
    if (opt.type == getopt_optional)
    {
      long_opt << "[=";
      if (opt.value_desc)
        long_opt << opt.value_desc;
      else
        long_opt << "arg";
      long_opt << ']';
    }
    if (opt.type == getopt_required)
    {
      long_opt << "=";
      if (opt.value_desc)
        long_opt << opt.value_desc;
      else
        long_opt << "arg";
    }
    out << std::setw(20) << std::left << long_opt.str();
    out << '\t';
    if (opt.desc)
    {
      out << opt.desc;
    }
    out << '\n';
  }
  return out;
}

} // namespace gpcl

#endif //
