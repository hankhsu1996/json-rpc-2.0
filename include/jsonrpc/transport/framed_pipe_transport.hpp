#pragma once

#include <asio.hpp>
#include <asio/local/stream_protocol.hpp>
#include <string>

#include "jsonrpc/transport/framed_transport.hpp"
#include "jsonrpc/transport/pipe_transport.hpp"

namespace jsonrpc {
namespace transport {

/**
 * @brief Transport layer using Asio Unix domain sockets for JSON-RPC
 * communication with framing.
 */
class FramedPipeTransport : public PipeTransport, protected FramedTransport {
public:
  FramedPipeTransport(const std::string &socketPath, bool isServer);

  void SendMessage(const std::string &message) override;
  std::string ReceiveMessage() override;
};

} // namespace transport
} // namespace jsonrpc
