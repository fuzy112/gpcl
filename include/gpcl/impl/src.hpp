//
// src.hpp
// ~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SOURCE
#define GPCL_SOURCE 1
#endif

#include <gpcl/detail/config.hpp>

#include <gpcl/detail/impl/assertion_failure.ipp>
#include <gpcl/detail/impl/debug_allocator.ipp>
#include <gpcl/detail/impl/get_mutex_for_address.ipp>
#include <gpcl/detail/impl/throw_system_error.ipp>
#include <gpcl/detail/impl/tlsf.ipp>
#include <gpcl/detail/impl/unreachable.ipp>
#include <gpcl/impl/dump_build_info.ipp>
#include <gpcl/impl/getopt.ipp>
#include <gpcl/impl/json_error.ipp>
#include <gpcl/impl/sqlite.ipp>
#include <gpcl/pmr/impl/default_resource.ipp>
#include <gpcl/pmr/impl/monotonic_buffer_resource.ipp>
#include <gpcl/pmr/impl/new_delete_resource.ipp>
#include <gpcl/pmr/impl/null_memory_resource.ipp>
#include <gpcl/pmr/impl/tlsf_resource.ipp>

#ifdef GPCL_POSIX
#include <gpcl/detail/impl/posix_clock.ipp>
#include <gpcl/detail/impl/posix_condition_variable.ipp>
#include <gpcl/detail/impl/posix_file.ipp>
#include <gpcl/detail/impl/posix_lock_file.ipp>
#include <gpcl/detail/impl/posix_message_queue.ipp>
#include <gpcl/detail/impl/posix_mutex.ipp>
#include <gpcl/detail/impl/posix_pid_file.ipp>
#include <gpcl/detail/impl/posix_semaphore.ipp>
#include <gpcl/detail/impl/posix_thread.ipp>
#endif

#ifdef GPCL_LINUX
#include <gpcl/detail/impl/futex.ipp>
#include <gpcl/detail/impl/futex_condition_variable.ipp>
#include <gpcl/detail/impl/futex_mutex.ipp>
#endif

#ifdef GPCL_WINDOWS
#include <gpcl/detail/impl/win_clock.ipp>
#include <gpcl/detail/impl/win_condition_variable.ipp>
#include <gpcl/detail/impl/win_file.ipp>
#include <gpcl/detail/impl/win_lock_file.ipp>
#include <gpcl/detail/impl/win_mutex.ipp>
#include <gpcl/detail/impl/win_semaphore.ipp>
#if !defined(GPCL_NO_STACKTRACE)
#  include <gpcl/detail/impl/win_stacktrace.ipp>
#endif
#include <gpcl/detail/impl/win_thread.ipp>
#endif

#if defined(GPCL_BFD) && !defined(GPCL_NO_STACKTRACE) && !defined(GPCL_LLVM)
#include <gpcl/detail/impl/bfd_stacktrace.ipp>
#endif

#if defined(__CYGWIN__) && !defined(GPCL_NO_STACKTRACE)
#include <gpcl/detail/impl/win_stacktrace.ipp>
#endif

#if defined(GPCL_LLVM) && !defined(GPCL_NO_STACKTRACE)
#include <gpcl/detail/impl/llvm_stacktrace.ipp>
#endif
