//
// get_deleter.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_GET_DELETER_HPP
#define GPCL_GET_DELETER_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

template <typename T>
class shared_ptr;

/// Returns the deleter of specified type, if owned.
/** @param p a shared pointer whose deleter needs to be accessed.
 *  @returns A pointer to the owned deleter or nullptr.  The returned pointer is
 * valid at least as long as there remains at least one shared_ptr instance that
 * owns it.
 *
 * @note The returned pointer may outlive the last shared_ptr if, for example,
 * `weak_ptr`s remain and the implementation doesn't destroy the deleter until
 * the entire control block is destroyed.
 * 
 * @relates gpcl::shared_ptr
 */
template <typename Deleter, typename T>
Deleter *get_deleter(const shared_ptr<T> &p) noexcept;

} // namespace gpcl

#include <gpcl/impl/get_deleter.hpp>

#endif // GPCL_GET_DELETER_HPP
