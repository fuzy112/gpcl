//
// mqtt.ipp
// ~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/ext/mqtt.hpp>
#include <cstring>

#ifdef GPCL_EXT_ENABLE_MOSQUITTO

namespace gpcl {
namespace ext {
namespace mqtt {

czstring<> mqtt_category_impl::name() const noexcept
{
  return "mqtt";
}

std::string mqtt_category_impl::message(int code) const
{
  if (code == 0)
  {
    return "success";
  }

  return mosquitto_strerror(code);
}

std::error_code make_error_code(errc errc_) noexcept
{
  return std::error_code(static_cast<int>(errc_), mqtt_category());
}

const std::error_category &mqtt_category() noexcept
{
  const static mqtt_category_impl impl;
  return impl;
}

client::client(czstring<> id, bool clean_session)
{
  mosq_ = mosquitto_new(id, clean_session, this);
  if (!mosq_)
  {
    if (errno == ENOMEM)
      throw std::system_error(
          std::make_error_code(std::errc::not_enough_memory));

    GPCL_UNREACHABLE("unknown error");
  }
}

client::~client() noexcept
{
  mosquitto_destroy(mosq_);
}

void client::set_will(czstring<> topic, gpcl::span<const char> payload,
                      qos t_qos, retain t_retain)
{
  int err = ::mosquitto_will_set(mosq_, topic, payload.size(), payload.data(),
                                 static_cast<int>(t_qos),
                                 static_cast<bool>(t_retain));
  switch (err)
  {
  case MOSQ_ERR_SUCCESS:
    return;

  case MOSQ_ERR_NOMEM:
  case MOSQ_ERR_PAYLOAD_SIZE:
    throw std::system_error(err, mqtt_category(), "mosquitto_will_set");

  case MOSQ_ERR_INVAL:
    GPCL_UNREACHABLE("invalid arguments");

  default:
    GPCL_UNREACHABLE("unknown error code");
  }
}

void client::clear_will()
{
  GPCL_VERIFY_(MOSQ_ERR_SUCCESS, mosquitto_will_clear(mosq_));
}

void client::username_and_password(czstring<> username, czstring<> password)
{
  std::error_code ec;
  username_and_password(username, password, ec);
  if (ec)
    throw std::system_error(ec, __func__);
}

void client::username_and_password(czstring<> username, czstring<> password,
                                   std::error_code &ec)
{
  GPCL_ASSERT(!connect_called_);

  int err = mosquitto_username_pw_set(mosq_, username, password);
  if (err == MOSQ_ERR_SUCCESS)
    return ec.clear();
  if (err == MOSQ_ERR_NOMEM)
  {
    return ec.assign(err, mqtt_category());
  }
  GPCL_UNREACHABLE("uknown error");
}

void client::connect(czstring<> host, std::uint16_t port, int keepalive_sec)
{
  std::error_code ec;
  connect(host, port, keepalive_sec, ec);
  if (ec)
    throw std::system_error(ec, __func__);
}

void client::connect(czstring<> host, std::uint16_t port, int keepalive_sec,
                     std::error_code &ec)
{
  connect_called_ = true;

  int err = ::mosquitto_connect(mosq_, host, port, keepalive_sec);
  if (err == MOSQ_ERR_SUCCESS)
    return ec.clear();
  if (err == MOSQ_ERR_ERRNO)
  {
    return ec.assign(errno, std::system_category());
  }
  GPCL_UNREACHABLE("unknown error");
}

void client::async_connect(czstring<> host, std::uint16_t port,
                           int keepalive_sec)
{
  std::error_code ec;
  async_connect(host, port, keepalive_sec, ec);
  if (ec)
    throw std::system_error(ec, __func__);
}

void client::async_connect(czstring<> host, std::uint16_t port,
                           int keepalive_sec, std::error_code &ec)
{
  connect_called_ = true;

  int err = ::mosquitto_connect_async(mosq_, host, port, keepalive_sec);
  if (err == MOSQ_ERR_SUCCESS)
    return ec.clear();
  if (err == MOSQ_ERR_ERRNO)
  {
    return ec.assign(errno, std::system_category());
  }
  if (err == MOSQ_ERR_INVAL)
  {
    return ec.assign(err, mqtt_category());
  }
  GPCL_UNREACHABLE("unknown error");
}

void client::disconnect()
{
  std::error_code ec;
  disconnect(ec);
  if (ec)
    throw std::system_error(ec, __func__);
}

void client::disconnect(std::error_code &ec)
{
  switch (int err = ::mosquitto_disconnect(mosq_))
  {
  case MOSQ_ERR_SUCCESS:
    return ec.clear();
  case MOSQ_ERR_NO_CONN:
    ec.assign(err, mqtt_category());
    return;

  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

void client::disconnect(int reason_code, const properties &props)
{
  GPCL_UNIMPLEMENTED();
}

void client::reconnect()
{
  std::error_code ec;
  reconnect(ec);
  if (ec)
    throw std::system_error(ec, __func__);
}

void client::reconnect(std::error_code &ec)
{
  GPCL_ASSERT(connect_called_);

  switch (int err = mosquitto_reconnect(mosq_))
  {
  case MOSQ_ERR_SUCCESS:
    return ec.clear();

  case MOSQ_ERR_NOMEM:
    return ec.assign(err, mqtt_category());

  case MOSQ_ERR_ERRNO:
    return ec.assign(errno, std::system_category());

  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

void client::async_reconnect()
{
  std::error_code ec;
  async_reconnect(ec);
  if (ec)
    throw std::system_error(ec, __func__);
}

void client::async_reconnect(std::error_code &ec)
{
  GPCL_ASSERT(connect_called_);

  switch (int err = mosquitto_reconnect_async(mosq_))
  {
  case MOSQ_ERR_SUCCESS:
    return ec.clear();

  case MOSQ_ERR_NOMEM:
    return ec.assign(err, mqtt_category());

  case MOSQ_ERR_ERRNO:
    return ec.assign(errno, std::system_category());

  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

message_id client::publish(czstring<> topic, gpcl::span<const char> payload,
                           qos t_qos, retain t_retain)
{
  std::error_code ec;
  auto ret = publish(topic, payload, t_qos, t_retain, ec);
  if (ec)
    throw std::system_error(ec, __func__);
  return ret;
}

message_id client::publish(czstring<> topic, gpcl::span<const char> payload,
                           qos t_qos, retain t_retain, std::error_code &ec)
{
  int mid;
  switch (int err = ::mosquitto_publish(mosq_, &mid, topic, payload.size(),
                                        payload.data(), static_cast<int>(t_qos),
                                        static_cast<bool>(t_retain)))
  {
  case MOSQ_ERR_SUCCESS:
    ec.clear();
    return message_id{mid};

  case MOSQ_ERR_NOMEM:
  case MOSQ_ERR_NO_CONN:
  case MOSQ_ERR_PROTOCOL:
  case MOSQ_ERR_PAYLOAD_SIZE:
  case MOSQ_ERR_MALFORMED_UTF8:
  case MOSQ_ERR_OVERSIZE_PACKET:
    ec.assign(err, mqtt_category());
    return message_id{};

  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

message_id client::subscribe(czstring<> pattern, qos t_qos)
{
  std::error_code ec;
  auto ret = subscribe(pattern, t_qos, ec);
  if (ec)
    throw std::system_error(ec, __func__);
  return ret;
}

message_id client::subscribe(czstring<> pattern, qos t_qos, std::error_code &ec)
{
  int mid;

  switch (int err = ::mosquitto_subscribe(mosq_, &mid, pattern,
                                          static_cast<int>(t_qos)))
  {
  case MOSQ_ERR_SUCCESS:
    ec.clear();
    return message_id{mid};

  case MOSQ_ERR_NOMEM:
  case MOSQ_ERR_NO_CONN:
  case MOSQ_ERR_MALFORMED_UTF8:
  case MOSQ_ERR_OVERSIZE_PACKET:
    ec.assign(err, mqtt_category());
    return message_id{};

  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

message_id client::unsubscribe(czstring<> pattern)
{
  std::error_code ec;
  auto ret = unsubscribe(pattern, ec);
  if (ec)
    throw std::system_error(ec, __func__);
  return ret;
}

message_id client::unsubscribe(czstring<> pattern, std::error_code &ec)
{
  int mid;

  switch (int err = ::mosquitto_unsubscribe(mosq_, &mid, pattern))
  {
  case MOSQ_ERR_SUCCESS:
    ec.clear();
    return message_id{mid};

  case MOSQ_ERR_NOMEM:
  case MOSQ_ERR_NO_CONN:
  case MOSQ_ERR_MALFORMED_UTF8:
  case MOSQ_ERR_OVERSIZE_PACKET:
    ec.assign(err, mqtt_category());
    return message_id{};

  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

void client::poll(int timeout_ms)
{
  std::error_code ec;
  poll(timeout_ms, ec);
  if (ec)
    throw std::system_error(ec, __func__);
}

void client::poll(int timeout_ms, std::error_code &ec)
{
  switch (int err = ::mosquitto_loop(mosq_, timeout_ms, 1))
  {
  case MOSQ_ERR_SUCCESS:
    return ec.clear();

  case MOSQ_ERR_NOMEM:
  case MOSQ_ERR_NO_CONN:
  case MOSQ_ERR_CONN_LOST:
  case MOSQ_ERR_PROTOCOL:
    return ec.assign(err, mqtt_category());

  case MOSQ_ERR_ERRNO:
    return ec.assign(errno, std::system_category());

  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

void client::loop_start()
{
  switch (::mosquitto_loop_start(mosq_))
  {
  case MOSQ_ERR_SUCCESS:
    return;
  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

void client::loop_stop()
{
  switch (::mosquitto_loop_stop(mosq_, false))
  {
  case MOSQ_ERR_SUCCESS:
    return;
  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

void client::loop_stop_force()
{
  switch (::mosquitto_loop_stop(mosq_, true))
  {
  case MOSQ_ERR_SUCCESS:
    return;
  default:
    GPCL_UNREACHABLE("unknown error");
  }
}

void client::protocol_version(client::protocol_version_type version)
{
  GPCL_ASSERT(!connect_called_);

  GPCL_VERIFY_(MOSQ_ERR_SUCCESS,
               mosquitto_int_option(mosq_, MOSQ_OPT_PROTOCOL_VERSION,
                                    static_cast<int>(version)));
}

void client::receive_maximum(std::uint16_t amount)
{
  GPCL_VERIFY_(MOSQ_ERR_SUCCESS,
               ::mosquitto_int_option(mosq_, MOSQ_OPT_RECEIVE_MAXIMUM, amount));
}

void client::send_maximum(std::uint16_t amount)
{
  GPCL_VERIFY_(MOSQ_ERR_SUCCESS,
               ::mosquitto_int_option(mosq_, MOSQ_OPT_SEND_MAXIMUM, amount));
}

void client::reconnect_delay(int delay_sec, int delay_max_sec,
                             bool exponential_backoff)
{
  GPCL_VERIFY_(MOSQ_ERR_SUCCESS,
               ::mosquitto_reconnect_delay_set(mosq_, delay_sec, delay_max_sec,
                                               exponential_backoff));
}

void client::connect_callback(std::function<void(mqtt_result)> callback)
{
  connect_cb_ = std::move(callback);
  mosquitto_connect_callback_set(mosq_, [](mosquitto *, void *self, int rc) {
    reinterpret_cast<client *>(self)->connect_cb_(static_cast<mqtt_result>(rc));
  });
}

void client::disconnect_callback(std::function<void(mqtt::errc)> callback)
{
  disconnected_cb_ = std::move(callback);
  mosquitto_disconnect_callback_set(mosq_, [](mosquitto *, void *self, int rc) {
    reinterpret_cast<client *>(self)->disconnected_cb_(
        static_cast<mqtt::errc>(rc));
  });
}

void client::publish_callback(std::function<void(message_id)> callback)
{
  publish_cb_ = std::move(callback);
  mosquitto_publish_callback_set(mosq_, [](mosquitto *, void *self, int mid) {
    reinterpret_cast<client *>(self)->publish_cb_(message_id{mid});
  });
}

void client::message_callback(std::function<void(message_ref)> callback)
{
  message_cb_ = std::move(callback);
  mosquitto_message_callback_set(
      mosq_, [](mosquitto *, void *self, const mosquitto_message *msg) {
        reinterpret_cast<client *>(self)->message_cb_(message_ref{msg});
      });
}

void client::subscribe_callback(
    std::function<void(message_id, gpcl::span<const qos>)> callback)
{
  subscribe_cb_ = std::move(callback);
  mosquitto_subscribe_callback_set(mosq_, [](mosquitto *, void *self, int mid,
                                             int qos_count,
                                             const int *granted_qos) {
    GPCL_ASSERT(qos_count <= 10);
    std::array<qos, 10> granted_qos_copy{};
    // here I copied the array to avoid type aliasing.
    // do we really need this since we didn't try to modify the data using both
    // types?
    std::memcpy(granted_qos_copy.data(), granted_qos, sizeof(int) * qos_count);

    reinterpret_cast<client *>(self)->subscribe_cb_(
        message_id{mid},
        gpcl::span<const qos>(granted_qos_copy.data(), qos_count));
  });
}

void client::unsubscribe_callback(std::function<void(message_id)> callback)
{
  unsubscribe_cb_ = std::move(callback);
  mosquitto_unsubscribe_callback_set(
      mosq_, [](mosquitto *, void *self, int mid) {
        reinterpret_cast<client *>(self)->unsubscribe_cb_(message_id{mid});
      });
}

void client::log_callback(
    std::function<void(mqtt_log_level, czstring<>)> callback)
{
  log_cb_ = std::move(log_cb_);
  mosquitto_log_callback_set(
      mosq_, [](mosquitto *, void *self, int level, czstring<> str) {
        reinterpret_cast<client *>(self)->log_cb_(
            static_cast<mqtt_log_level>(level), str);
      });
}

void client::threaded(bool enabled)
{
  GPCL_VERIFY_(MOSQ_ERR_SUCCESS, mosquitto_threaded_set(mosq_, enabled));
}

bool client::want_write()
{
  GPCL_UNIMPLEMENTED();
}

int client::socket()
{
  GPCL_UNIMPLEMENTED();
}

void client::loop_misc()
{
  GPCL_UNIMPLEMENTED();
}

void client::loop_write()
{
  GPCL_UNIMPLEMENTED();
}

void client::loop_read()
{
  GPCL_UNIMPLEMENTED();
}

void client::loop_forever()
{
  GPCL_UNIMPLEMENTED();
}

void client::set_will(czstring<> topic, gpcl::span<const char> payload,
                      qos t_qos, retain t_retain, properties props)
{
  GPCL_UNIMPLEMENTED();
}

message message_ref::to_owned() const
{
  mosquitto_message msg{};
  int err = mosquitto_message_copy(&msg, msg_);
  if (err == MOSQ_ERR_NOMEM)
    throw std::system_error(err, mqtt_category(), "mosquitto_message_copy");
  if (err == MOSQ_ERR_SUCCESS)
    return message{std::move(msg)};
  GPCL_UNREACHABLE("unknown error");
}

message::~message()
{
  mosquitto_message_free_contents(this);
}

message_ref message::as_ref() const
{
  return message_ref{this};
}

message message::clone() const
{
  return as_ref().to_owned();
}

namespace detail {
inline static std::atomic<int> init_count{};
}

mqtt_init_guard::mqtt_init_guard()
{
  if (detail::init_count.fetch_add(1) == 0)
  {
    GPCL_VERIFY_0(mosquitto_lib_init());
  }
}

mqtt_init_guard::~mqtt_init_guard() noexcept
{
  if (detail::init_count.fetch_sub(1) == 1)
  {
    GPCL_VERIFY_0(mosquitto_lib_cleanup());
  }
}

namespace {
[[maybe_unused]] mqtt_init_guard mqtt_lib_init;
}

} // namespace mqtt
} // namespace ext
} // namespace gpcl

#endif
