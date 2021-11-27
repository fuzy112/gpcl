//
// message_queue.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_MESSAGE_QUEUE_HPP
#define GPCL_MESSAGE_QUEUE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_message_queue.hpp>
#include <gpcl/expected.hpp>

namespace gpcl {
#ifdef GPCL_POSIX

class message_queue
{
#  ifdef GPCL_POSIX
  using impl_type = detail::posix_message_queue;
#  endif

  impl_type impl_;

public:
  message_queue(open_only_t, czstring<> name) : impl_(open_only, name) {}

  message_queue(open_or_create_t, czstring<> name, std::size_t maxmsg,
                std::size_t msgsize)
      : impl_(open_or_create, name, maxmsg, msgsize)
  {
  }

  message_queue(create_only_t, czstring<> name, std::size_t maxmsg,
                std::size_t msgsize)
      : impl_(create_only, name, maxmsg, msgsize)
  {
  }

  static void unlink(czstring<> name) { impl_type ::unlink(name); }

  static void unlink(czstring<> name, std::error_code &ec)
  {
    impl_type ::unlink(name, ec);
  }

  static gpcl::expected<void, std::error_code> unlink(use_expected_t,
                                                      czstring<> name)
  {
    std::error_code ec;
    impl_type ::unlink(name, ec);
    if (ec)
      return gpcl::make_unexpected(ec);
    return {};
  }

  void send(span<const char> msg, unsigned int prio)
  {
    return impl_.send(msg, prio);
  }

  void send(span<const char> msg, unsigned int prio, std::error_code &ec)
  {
    return impl_.send(msg, prio, ec);
  }

  gpcl::expected<void, std::error_code>
  send(use_expected_t, span<const char> msg, unsigned int prio)
  {
    std::error_code ec;
    impl_.send(msg, prio, ec);
    if (ec)
      return gpcl::make_unexpected(ec);
    return {};
  }

  std::size_t receive(span<char> msg) { return impl_.receive(msg); }

  std::size_t receive(span<char> msg, std::error_code &ec)
  {
    return impl_.receive(msg, ec);
  }

  gpcl::expected<span<char>, std::error_code> receive(use_expected_t,
                                                      span<char> msg)
  {
    std::error_code ec;
    auto len = impl_.receive(msg, ec);
    if (ec)
      return gpcl::make_unexpected(ec);
    return msg.subspan(0, len);
  }
};

#endif

} // namespace gpcl

#endif
