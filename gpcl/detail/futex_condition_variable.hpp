//
// futex_condition_variable.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_FUTEX_CONDITION_VARIABLE_HPP
#define GPCL_DETAIL_FUTEX_CONDITION_VARIABLE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/futex.hpp>
#include <gpcl/detail/futex_mutex.hpp>

namespace gpcl {
namespace detail {

class futex_condition_variable
{
public:
  inline constexpr explicit futex_condition_variable() noexcept;

  futex_condition_variable(const futex_condition_variable &) = delete;
  futex_condition_variable(futex_condition_variable &&) = delete;

  GPCL_DECL void wait(futex_mutex &mtx);

  GPCL_DECL void notify_one();

  GPCL_DECL void notify_all();

private:
  futex_word_type fut_;
};

constexpr futex_condition_variable::futex_condition_variable() noexcept
    : fut_(0)
{
}

} // namespace detail
} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/futex_condition_variable.ipp>
#endif

#endif // GPCL_DETAIL_FUTEX_CONDITION_VARIABLE_HPP
