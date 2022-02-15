//
// ref_count_base.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_REF_COUNT_BASE_HPP
#define GPCL_DETAIL_REF_COUNT_BASE_HPP

#include <gpcl/detail/config.hpp>

#undef __cpp_lib_atomic_is_always_lock_free

#if defined __cpp_lib_atomic_is_always_lock_free
#  include <gpcl/detail/std_ref_count_base.hpp>
#elif defined _MSC_VER
#  include <gpcl/detail/msvc_ref_count_base.hpp>
#elif defined(__GNUC__)
#  include <gpcl/detail/gcc_ref_count_base.hpp>
#endif


namespace gpcl {
namespace detail {

#ifdef GPCL_DOXYGEN

/// Base class of shared_block<>.
/// This class implements the reference counting functionality.
class ref_count_base
{
public:
  typedef void *(*operation_func_t)(ref_count_base *self,
                                    ref_count_operation) noexcept;

  /// Increment use count.
  void get() noexcept;

  /// Decrement use count and if use count reaches zero, destroy the managed
  /// object. After the managed object is destroyed, the weak count will be
  /// decremented. If weak count reaches zero, the control block will be
  /// destroyed.
  void put() noexcept;

  /// Returns the use count.
  long use_count() const noexcept;

  /// Increment weak count.
  void weak_get() noexcept;

  /// Decrement weak count.
  /// If weak count reaches zero, the control block will be destroyed.
  void weak_put() noexcept;

  /// Returns the weak count.
  long weak_count() const noexcept;

  /// Try increment the use count, fail if use count equals 0.
  bool lock() noexcept;

  /// Invoke an operation.
  void operate(ref_count_operation op) noexcept;

protected:
  /// Constructor.
  explicit ref_count_base(operation_func_t op_func);
};

#elif defined __cpp_lib_atomic_is_always_lock_free
typedef std_ref_count_base ref_count_base;
#elif defined _MSC_VER
typedef msvc_ref_count_base ref_count_base;
#elif defined(__GNUC__)
typedef gcc_ref_count_base ref_count_base;
#endif

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_REF_COUNT_BASE_HPP
