//
// posix_message_queue.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_MESSAGE_QUEUE_HPP
#define GPCL_DETAIL_POSIX_MESSAGE_QUEUE_HPP

#include <gpcl/creation_tag.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/expected_fwd.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/span.hpp>
#include <gpcl/zstring.hpp>
#include <gpcl/detail/unique_handle.hpp>

#include <cstdint>
#include <memory>

#include <mqueue.h>

namespace gpcl {
namespace detail {

struct mqd_traits
{
  using native_handle_type = mqd_t;

  static inline const native_handle_type invalid_value{(mqd_t)-1};

  static bool is_valid(mqd_t q) noexcept { return q >= (mqd_t)0; }

  static void close(native_handle_type q) noexcept
  {
    GPCL_VERIFY_0(mq_close(q));
  }
};

using unique_mqd = unique_handle<mqd_traits>;

class posix_message_queue
{
  friend class allocated_message;

public:
  using size_type = std::size_t;

  GPCL_DECL posix_message_queue(create_only_t, czstring<> name,
                                std::size_t maxmsg, std::size_t msgsize);
  GPCL_DECL posix_message_queue(open_or_create_t, czstring<> name,
                                std::size_t maxmsg, std::size_t msgsize);
  GPCL_DECL posix_message_queue(open_only_t, czstring<> name);

  posix_message_queue() noexcept = default;

  GPCL_DECL static void unlink(czstring<> name, std::error_code &ec);

  static void unlink(czstring<> name)
  {
    std::error_code ec;
    posix_message_queue::unlink(name, ec);
    if (ec)
      GPCL_THROW(std::system_error(ec, __PRETTY_FUNCTION__));
  }

  GPCL_DECL void send(gpcl::span<const char> msg, unsigned int prio,
                      std::error_code &ec);

  void send(gpcl::span<const char> msg, unsigned int prio)
  {
    std::error_code ec;
    this->send(msg, prio, ec);
    if (ec)
      GPCL_THROW(std::system_error(ec, __PRETTY_FUNCTION__));
  }

  GPCL_DECL std::size_t receive(gpcl::span<char> msg, std::error_code &ec);

  std::size_t receive(gpcl::span<char> msg)
  {
    std::error_code ec;
    auto ret = this->receive(msg, ec);
    if (ec)
      GPCL_THROW(std::system_error(ec, __PRETTY_FUNCTION__));
    return ret;
  }

private:
  unique_mqd q_;
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_POSIX_posix_message_queue_HPP
