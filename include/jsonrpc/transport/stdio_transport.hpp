#pragma once

#include <string>

#include "jsonrpc/transport/transport.hpp"

namespace jsonrpc::transport {

/**
 * @brief Transport layer using standard I/O for JSON-RPC communication.
 */
class StdioTransport : public Transport {
 public:
  void SendMessage(const std::string &message) override;
  auto ReceiveMessage() -> std::string override;
};

}  // namespace jsonrpc::transport
