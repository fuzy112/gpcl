//
// gpcl.hpp
// ~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_HPP
#define GPCL_HPP

#include <gpcl/allocate_shared.hpp>
#include <gpcl/any.hpp>
#include <gpcl/any_cast.hpp>
#include <gpcl/assert.hpp>
#include <gpcl/bad_any_cast.hpp>
#include <gpcl/bad_weak_ptr.hpp>
#include <gpcl/basic_any.hpp>
#include <gpcl/buffer.hpp>
#include <gpcl/buffer_sequence.hpp>
#include <gpcl/clock.hpp>
#include <gpcl/clone_ptr.hpp>
#include <gpcl/condition_variable.hpp>
#include <gpcl/creation_tag.hpp>
#include <gpcl/decay_copy.hpp>
#include <gpcl/enable_shared_from_this.hpp>
#include <gpcl/error.hpp>
#include <gpcl/event.hpp>
#include <gpcl/expected.hpp>
#include <gpcl/expected_fwd.hpp>
#include <gpcl/file.hpp>
#include <gpcl/function.hpp>
#include <gpcl/future.hpp>
#include <gpcl/in_place.hpp>
#include <gpcl/in_place_type.hpp>
#include <gpcl/intrusive_list.hpp>
#include <gpcl/inttypes.hpp>
#include <gpcl/is_basic_lockable.hpp>
#include <gpcl/is_lockable.hpp>
#include <gpcl/lock_file.hpp>
#include <gpcl/make_any.hpp>
#include <gpcl/make_basic_any.hpp>
#include <gpcl/make_shared.hpp>
#include <gpcl/message_queue.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/narrow_cast.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/object_pool.hpp>
#include <gpcl/offset_ptr.hpp>
#include <gpcl/once_flag.hpp>
#include <gpcl/optional.hpp>
#include <gpcl/optional_fwd.hpp>
#include <gpcl/pid_file.hpp>
#include <gpcl/pool.hpp>
#include <gpcl/pool_allocator.hpp>
#include <gpcl/propagate_const.hpp>
#include <gpcl/semaphore.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/simple_segregated_storage.hpp>
#include <gpcl/span.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/thread_annotations.hpp>
#include <gpcl/thread_attributes.hpp>
#include <gpcl/time.hpp>
#include <gpcl/unexpected.hpp>
#include <gpcl/unique_lock.hpp>
#include <gpcl/unique_ptr.hpp>
#include <gpcl/unique_resource.hpp>
#include <gpcl/vector.hpp>
#include <gpcl/version.hpp>
#include <gpcl/weak_ptr.hpp>
#include <gpcl/zstring.hpp>

#endif // GPCL_HPP
