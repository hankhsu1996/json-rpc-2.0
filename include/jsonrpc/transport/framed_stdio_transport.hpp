#pragma once

#include <string>

#include "jsonrpc/transport/framed_transport.hpp"
#include "jsonrpc/transport/transport.hpp"

namespace jsonrpc {
namespace transport {

/**
 * @brief Transport layer using framed messaging over standard I/O for JSON-RPC
 * communication.
 *
 * This class uses framed transport to send and receive JSON-RPC messages over
 * standard I/O.
 */
class FramedStdioTransport : public Transport, protected FramedTransport {
public:
  void SendMessage(const std::string &request) override;
  std::string ReceiveMessage() override;
};

} // namespace transport
} // namespace jsonrpc
