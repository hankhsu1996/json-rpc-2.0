#pragma once

#include <asio.hpp>
#include <asio/local/stream_protocol.hpp>
#include <cstdint>
#include <string>

#include "jsonrpc/transport/framed_transport.hpp"
#include "jsonrpc/transport/socket_transport.hpp"

namespace jsonrpc::transport {

/**
 * @brief Transport layer using Asio sockets for JSON-RPC
 * communication with framing.
 */
class FramedSocketTransport : public SocketTransport,
                              protected FramedTransport {
 public:
  FramedSocketTransport(const std::string &host, uint16_t port, bool is_server);

  void SendMessage(const std::string &message) override;
  auto ReceiveMessage() -> std::string override;
};

}  // namespace jsonrpc::transport
