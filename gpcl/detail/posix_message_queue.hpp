//
// posix_message_queue.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
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
#include <cstdint>
#include <memory>
#include <system_error>

#ifdef GPCL_POSIX
#  include <mqueue.h>

namespace gpcl {
namespace detail {

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

  inline posix_message_queue() : q_(-1) {}

  inline posix_message_queue(posix_message_queue &&other) noexcept
      : q_(std::exchange(other.q_, -1))
  {
  }

  inline posix_message_queue &operator=(posix_message_queue &&other) noexcept
  {
    using gpcl::swap;
    swap(q_, other.q_);
    return *this;
  }

  GPCL_DECL ~posix_message_queue() noexcept;

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
  mqd_t q_;
};

} // namespace detail
} // namespace gpcl

#endif

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/posix_message_queue.ipp>
#endif

#endif // GPCL_POSIX_posix_message_queue_HPP
