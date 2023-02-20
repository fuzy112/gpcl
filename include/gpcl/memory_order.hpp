#ifndef GPCL_MEMORY_ORDER_HPP
#define GPCL_MEMORY_ORDER_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

#if GPCL_GCC || GPCL_CLANG
enum memory_order : int
{
  relaxed = __ATOMIC_RELAXED,
  consume = __ATOMIC_CONSUME,
  acquire = __ATOMIC_ACQUIRE,
  release = __ATOMIC_RELEASE,
  acq_rel = __ATOMIC_ACQ_REL,
  seq_cst = __ATOMIC_SEQ_CST,
};

#else
enum memory_order
{
  relaxed,
  consume,
  acquire,
  release,
  acq_rel,
  seq_cst,
};
#endif
} // namespace gpcl

#endif
