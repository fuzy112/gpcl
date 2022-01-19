//
// posix_message_queue.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_MESSAGE_QUEUE_IPP
#define GPCL_DETAIL_IMPL_POSIX_MESSAGE_QUEUE_IPP

#include <gpcl/detail/posix_message_queue.hpp>

namespace gpcl {
namespace detail {

using namespace std::string_literals;

posix_message_queue::posix_message_queue(create_only_t, czstring<> name,
                                         std::size_t maxmsg,
                                         std::size_t msgsize)
{
  mq_attr attr;
  attr.mq_maxmsg = maxmsg;
  attr.mq_msgsize = msgsize;

  this->q_ = mq_open(name, O_RDWR | O_CREAT | O_EXCL, 0666, &attr);
  if (this->q_ == -1)
  {
    throw_system_error(__PRETTY_FUNCTION__);
  }
}

posix_message_queue::posix_message_queue(open_or_create_t, czstring<> name,
                                         std::size_t maxmsg,
                                         std::size_t msgsize)
{
  mq_attr attr;
  attr.mq_maxmsg = maxmsg;
  attr.mq_msgsize = msgsize;

  this->q_ = mq_open(name, O_RDWR | O_CREAT, 0666, &attr);
  if (this->q_ == -1)
  {
    throw_system_error(__PRETTY_FUNCTION__);
  }
}

posix_message_queue::posix_message_queue(open_only_t, czstring<> name)
{
  this->q_ = mq_open(name, O_RDWR);
  if (this->q_ == -1)
  {
    throw_system_error(__PRETTY_FUNCTION__);
  }
}

posix_message_queue::~posix_message_queue() noexcept
{
  if (q_ != -1)
  {
    GPCL_VERIFY_0(::mq_close(q_));
  }
}

void posix_message_queue::unlink(czstring<> name, std::error_code &ec)
{
  GPCL_ASSERT(name != nullptr);
  if (-1 == ::mq_unlink(name))
  {
    ec.assign(errno, std::system_category());
  }
  else
  {
    ec.clear();
  }
}

void posix_message_queue::send(gpcl::span<const char> msg, unsigned int prio,
                               std::error_code &ec)
{
  GPCL_ASSERT(q_ != -1);
  if (-1 == mq_send(q_, msg.data(), msg.size_bytes(), prio))
  {
    return ec.assign(errno, std::system_category());
  }
  return ec.clear();
}

std::size_t posix_message_queue::receive(gpcl::span<char> msg,
                                         std::error_code &ec)
{
  GPCL_ASSERT(q_ != -1);
  unsigned int prio{};
  ssize_t len = mq_receive(q_, msg.data(), msg.size_bytes(), &prio);
  if (-1 == len)
  {
    ec.assign(errno, std::system_category());
    return {};
  }
  ec.clear();
  return len;
}

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_MESSAGE_QUEUE_IPP
