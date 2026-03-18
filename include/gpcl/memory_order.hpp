//
// memory_order.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MEMORY_ORDER_HPP
#define GPCL_MEMORY_ORDER_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

#if GPCL_GCC || GPCL_CLANG
enum class memory_order : int
{
  relaxed = __ATOMIC_RELAXED,
  consume = __ATOMIC_CONSUME,
  acquire = __ATOMIC_ACQUIRE,
  release = __ATOMIC_RELEASE,
  acq_rel = __ATOMIC_ACQ_REL,
  seq_cst = __ATOMIC_SEQ_CST,
};

#else
enum class memory_order : int
{
  relaxed = 0x00,
  consume = 0x01,
  acquire = 0x03,
  release = 0x04,
  acq_rel = 0x07,
  seq_cst = 0x0f,
};
#endif

constexpr auto memory_order_relaxed = memory_order::relaxed;
constexpr auto memory_order_consume = memory_order::consume;
constexpr auto memory_order_acquire = memory_order::acquire;
constexpr auto memory_order_release = memory_order::release;
constexpr auto memory_order_acq_rel = memory_order::acq_rel;
constexpr auto memory_order_seq_cst = memory_order::seq_cst;

} // namespace gpcl

#endif
