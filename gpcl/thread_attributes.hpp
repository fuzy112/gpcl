//
// thread_attributes.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_THREAD_ATTRIBUTES_HPP
#define GPCL_THREAD_ATTRIBUTES_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>

namespace gpcl {

/// Thread schedule policies.
enum class schedule_policy
{
  other,
  round_robin,
  fifo,
};

/// Thread attributes.
class thread_attributes : noncopyable
{
public:
  using schedule_policy_type = gpcl::schedule_policy;

  auto stack_size(int s) noexcept -> thread_attributes &
  {
    stack_size_ = s;
    return *this;
  }
  [[nodiscard]] auto stack_size() const noexcept -> int { return stack_size_; }

  auto priority(int p) noexcept -> thread_attributes &
  {
    priority_ = p;
    return *this;
  }
  [[nodiscard]] auto priority() const noexcept -> int { return priority_; }

  auto schedule_policy(schedule_policy_type s) noexcept -> thread_attributes &
  {
    schedule_policy_ = s;
    return *this;
  }
  [[nodiscard]] auto schedule_policy() const noexcept -> schedule_policy_type
  {
    return schedule_policy_;
  }

private:
  int stack_size_{32 * 1024 * 1024};
  int priority_{0};
  schedule_policy_type schedule_policy_{schedule_policy_type::other};
};

} // namespace gpcl
#endif
