//
// mqtt.hpp
// ~~~~~~~~
//
// Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EXT_MQTT_HPP
#define GPCL_EXT_MQTT_HPP

#include <gpcl/assert.hpp>
#include <gpcl/ext/detail/config.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/span.hpp>
#include <gpcl/zstring.hpp>
#include <atomic>
#include <functional>
#include <string>
#include <system_error>

#if defined GPCL_EXT_ENABLE_MOSQUITTO || defined GPCL_DOXYGEN
#  include <mosquitto.h>

#  ifdef GPCL_EXT_MQTT_ENABLE_SSL_HPP
#    include <openssl/ssl.h>
#  endif

namespace gpcl::ext::mqtt {
enum class errc
{
  auth_continue = MOSQ_ERR_AUTH_CONTINUE,
  no_subscribers = MOSQ_ERR_NO_SUBSCRIBERS,
  subscription_exists = MOSQ_ERR_SUB_EXISTS,
  connection_pending = MOSQ_ERR_CONN_PENDING,
  success = MOSQ_ERR_SUCCESS,
  no_memory = MOSQ_ERR_NOMEM,
  protocol_error = MOSQ_ERR_PROTOCOL,
  invalid_argument = MOSQ_ERR_INVAL,
  no_connection = MOSQ_ERR_NO_CONN,
  connection_refused = MOSQ_ERR_CONN_REFUSED,
  not_found = MOSQ_ERR_NOT_FOUND,
  connection_lost = MOSQ_ERR_CONN_LOST,
  tls_error = MOSQ_ERR_TLS,
  payload_too_large = MOSQ_ERR_PAYLOAD_SIZE,
  not_supported = MOSQ_ERR_NOT_SUPPORTED,
  authentication_error = MOSQ_ERR_AUTH,
  acl_denied = MOSQ_ERR_ACL_DENIED,
  unknown = MOSQ_ERR_UNKNOWN,
  errno_ = MOSQ_ERR_ERRNO,
  eai = MOSQ_ERR_EAI,
  proxy = MOSQ_ERR_PROXY,
  plugin_defer = MOSQ_ERR_PLUGIN_DEFER,
  malformed_utf8 = MOSQ_ERR_MALFORMED_UTF8,
  keepalive = MOSQ_ERR_KEEPALIVE,
  lookup = MOSQ_ERR_LOOKUP,
  malformed_packet = MOSQ_ERR_MALFORMED_PACKET,
  duplicate_property = MOSQ_ERR_DUPLICATE_PROPERTY,
  tls_handshake = MOSQ_ERR_TLS_HANDSHAKE,
  qos_not_supported = MOSQ_ERR_QOS_NOT_SUPPORTED,
  oversize_packet = MOSQ_ERR_OVERSIZE_PACKET,
  ocsp = MOSQ_ERR_OCSP,
};

GPCL_DECL
std::error_code make_error_code(errc errc_) noexcept;

GPCL_DECL
const std::error_category &mqtt_category() noexcept;

class mqtt_category_impl : public std::error_category
{
public:
  [[nodiscard]] GPCL_DECL czstring<> name() const noexcept override;

  [[nodiscard]] GPCL_DECL std::string message(int code) const override;
};
} // namespace gpcl::ext::mqtt

namespace std {
template <>
struct is_error_code_enum<gpcl::ext::mqtt::errc> : std::true_type
{
};

} // namespace std

namespace gpcl::ext::mqtt {

/// RAII guard used for initialization of libmosquitto
class mqtt_init_guard : gpcl::noncopyable
{
public:
  /// Initializes libmosquitto
  GPCL_DECL mqtt_init_guard();

  /// Cleanups libmosquitto
  GPCL_DECL ~mqtt_init_guard() noexcept;
};

/// MQTT QOS type
enum class qos : int
{
  at_most_once = 0,
  at_least_once = 1,
  exactly_once = 2,
};

/// Retain
enum retain : bool
{
  on = true,
  off = false,
};

/// Properties for MQTT V5
/// TODO
struct properties
{
};

/// Wrapper of message id
struct message_id
{
  int value;
};

/// MQTT CONACK result code
enum class mqtt_result
{
  success = 0,
  unacceptable_protocol_version = 1,
  identifier_rejected = 2,
  broker_unavailable = 3,
};

struct message_ref;

/// Owned message
struct message : mosquitto_message, gpcl::noncopyable
{
public:
  /// Construct from raw C mosquitto_message struct.
  ///
  /// Takes the ownership.
  explicit message(mosquitto_message &&msg) : mosquitto_message(msg) {}

  /// Destroy and release the message
  GPCL_DECL ~message();

  /// Convert to message_ref
  GPCL_DECL [[nodiscard]] message_ref as_ref() const;

  /// Clone a message
  GPCL_DECL [[nodiscard]] message clone() const;
};

/// MQTT log levels from libmosquitto
enum class mqtt_log_level
{
  info = MOSQ_LOG_INFO,
  notice = MOSQ_LOG_NOTICE,
  warning = MOSQ_LOG_WARNING,
  err = MOSQ_LOG_ERR,
  debug = MOSQ_LOG_DEBUG
};

/// Immutable unowned message reference
struct message_ref
{
  mosquitto_message const *msg_;

  [[nodiscard]] message_id id() const { return {msg_->mid}; }

  [[nodiscard]] czstring<> topic() const { return msg_->topic; }

  [[nodiscard]] span<const char> payload() const
  {
    return span<const char>(reinterpret_cast<const char *>(msg_->payload),
                            msg_->payloadlen);
  }

  using qos_type = mqtt::qos;

  [[nodiscard]] qos_type qos() const
  {
    return static_cast<qos_type>(msg_->qos);
  }

  using retain_type = mqtt::retain;

  [[nodiscard]] retain_type retain() const
  {
    return static_cast<retain_type>(msg_->retain);
  }

  /// Clone a owned message.
  [[nodiscard]] GPCL_DECL message to_owned() const;
};

/// C++ wrapper of mosquitto object
class client : mqtt_init_guard
{
  mosquitto *mosq_;

  // for debug
  bool connect_called_{};

  // callbacks
  std::function<void(mqtt_result)> connect_cb_;
  std::function<void(errc)> disconnected_cb_;
  std::function<void(message_id)> publish_cb_;
  std::function<void(message_ref)> message_cb_;
  std::function<void(message_id, gpcl::span<const qos>)> subscribe_cb_;
  std::function<void(message_id)> unsubscribe_cb_;
  std::function<void(mqtt_log_level, const char *)> log_cb_;

public:
  /// Create a mqtt client
  GPCL_DECL client(const char *id, bool clean_session);

  /// Destroy the mqtt client
  GPCL_DECL ~client() noexcept;

  /// Set the will message
  GPCL_DECL void set_will(czstring<> topic, span<const char> payload, qos t_qos,
                          retain t_retain);

  /// Set the will message for MQTT V5
  GPCL_DECL void set_will(czstring<> topic, span<const char> payload, qos t_qos,
                          retain t_retain, properties props);

  /// Clear the will message
  GPCL_DECL void clear_will();

  /// Set username and password
  ///
  /// \param username if null, no authentication is used
  /// \param password if null, only send the username to server
  GPCL_DECL void username_and_password(czstring<> username, czstring<> password,
                                       std::error_code &ec);

  /// Set username and password
  ///
  /// \param username if null, no authentication is used
  /// \param password if null, only send the username to server
  GPCL_DECL void username_and_password(czstring<> username,
                                       czstring<> password);

  /// Equivalent to username_and_password(nullptr, nullptr).
  inline void disable_authentication()
  {
    username_and_password(nullptr, nullptr);
  }

  /// Synchronously connects to the broker
  ///
  /// Use this function with `poll()` or `loop_forever()`.
  GPCL_DECL void connect(czstring<> host, std::uint16_t port, int keepalive_sec,
                         std::error_code &ec);

  /// Synchronously connects to the broker
  ///
  /// Use this function with `poll()` or `loop_forever()`.
  GPCL_DECL void connect(czstring<> host, std::uint16_t port,
                         int keepalive_sec);
  //  void connect_bind()

  /// Asynchronously connects to the broker
  ///
  /// Use this function with 'loop_start()`.
  GPCL_DECL void async_connect(czstring<> host, std::uint16_t port,
                               int keepalive_sec, std::error_code &ec);

  /// Asynchronously connects to the broker
  ///
  /// Use this function with 'loop_start()`.
  GPCL_DECL void async_connect(czstring<> host, std::uint16_t port,
                               int keepalive_sec);

  /// Disconnects from the broker.
  ///
  /// Cause the disconnect callback to be invoked with reason code 0.
  GPCL_DECL void disconnect(std::error_code &ec);

  /// Disconnects from the broker.
  ///
  /// Cause the disconnect callback to be invoked with reason code 0.
  GPCL_DECL void disconnect();

  /// Disconnects from the broker for MQTT V5.
  GPCL_DECL void disconnect(int reason_code, properties const &props);

  /// Synchronously disconnects and connects to the broker using the previous
  /// host and port.
  GPCL_DECL void reconnect(std::error_code &ec);

  /// Synchronously disconnects and connects to the broker using the previous
  /// host and port.
  GPCL_DECL void reconnect();

  /// Synchronously disconnects and connects to the broker using the previous
  /// host and port.
  GPCL_DECL void async_reconnect(std::error_code &ec);

  /// Synchronously disconnects and connects to the broker using the previous
  /// host and port.
  GPCL_DECL void async_reconnect();

  /// Publishes a message.
  GPCL_DECL message_id publish(czstring<> topic, span<const char> payload,
                               qos t_qos, retain t_retain, std::error_code &ec);

  /// Publishes a message.
  GPCL_DECL message_id publish(czstring<> topic, span<const char> payload,
                               qos t_qos, retain t_retain);

  /// Subscribes to a topic.
  GPCL_DECL message_id subscribe(czstring<> pattern, qos t_qos,
                                 std::error_code &ec);

  /// Subscribes to a topic.
  GPCL_DECL message_id subscribe(czstring<> pattern, qos t_qos);

  /// Unsubscribes a topic
  GPCL_DECL message_id unsubscribe(czstring<> pattern, std::error_code &ec);

  /// Unsubscribes a topic
  GPCL_DECL message_id unsubscribe(czstring<> pattern);

  /// @name loop managed by mosquitto
  /// @{

  /// This function must be called frequently to send and receive MQTT messages.
  GPCL_DECL void poll(int timeout_ms, std::error_code &ec);

  /// This function must be called frequently to send and receive MQTT messages.
  GPCL_DECL void poll(int timeout_ms);

  /// A loop that automatically calls poll().
  GPCL_DECL void loop_forever();

  /// Starts asynchronous loop
  GPCL_DECL void loop_start(); // async api

  /// Stops the asynchronous loop
  GPCL_DECL void loop_stop(); // async api;

  /// Forces stopping the asynchronous loop
  GPCL_DECL void loop_stop_force(); // async api

  /// @}

  /// @{ loop for integration to other event loops
  GPCL_DECL void loop_read();

  GPCL_DECL void loop_write();

  GPCL_DECL void loop_misc();

  /// Returns the socket
  GPCL_DECL int socket();

  /// Returns true if there is data ready to be written on the socket.
  GPCL_DECL bool want_write();

  /// @}

  /// Used to tell the library that your application is using threads, but not
  /// using mosquitto_loop_start.  The library operates slightly differently
  /// when not in threaded mode in order to simplify its operation.  If you are
  /// managing your own threads and do not use this function you will experience
  /// crashes due to race conditions
  /// When using mosquitto_loop_start, this is set automatically.
  GPCL_DECL void threaded(bool enabled);

  /// Protocol version
  enum protocol_version_type
  {
    mqtt_protocol_v31 = MQTT_PROTOCOL_V31,
    mqtt_protocol_v311 = MQTT_PROTOCOL_V311,
    mqtt_protocol_v5 = MQTT_PROTOCOL_V5,
  };

  /// Set mqtt protocol version
  GPCL_DECL void protocol_version(protocol_version_type version);

#  ifdef SHDQ_DEMO_ENABLE_SSL
  GPCL_DECL void ssl_ctx(SSL_CTX *ctx);

  GPCL_DECL void ssl_ctx_with_default(bool v);
#  endif

  /// Maximum message count to receive in one call to poll()
  GPCL_DECL void receive_maximum(uint16_t amount);

  /// Maximum message count to send in one call to poll()
  GPCL_DECL void send_maximum(uint16_t amount);

  GPCL_DECL void reconnect_delay(int delay_sec, int delay_max_sec,
                                 bool exponential_backoff);

  /// @name callbacks
  /// @{

  GPCL_DECL void connect_callback(std::function<void(mqtt_result)> callback);

  GPCL_DECL void disconnect_callback(std::function<void(errc)> callback);

  GPCL_DECL void publish_callback(std::function<void(message_id)> callback);

  GPCL_DECL void message_callback(std::function<void(message_ref)> callback);

  GPCL_DECL void subscribe_callback(
      std::function<void(message_id, gpcl::span<const qos>)> callback);

  GPCL_DECL void unsubscribe_callback(std::function<void(message_id)> callback);

  GPCL_DECL void
  log_callback(std::function<void(mqtt_log_level, czstring<>)> callback);

  /// @}
};

/// RAII guard for mosquitto async loop
struct async_loop_guard : gpcl::noncopyable
{
  client *client_;

  inline explicit async_loop_guard(client &client) : client_(&client)
  {
    client_->loop_start();
  }

  inline ~async_loop_guard() noexcept { client_->loop_stop(); }
};

} // namespace gpcl::ext::mqtt

#endif

#endif
