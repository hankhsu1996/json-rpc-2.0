#pragma once

#include <string>

#include "jsonrpc/transport/transport.hpp"

namespace jsonrpc {
namespace transport {

/**
 * @brief Transport layer using standard I/O for JSON-RPC communication.
 */
class StdioTransport : public Transport {
public:
  void SendMessage(const std::string &request) override;
  std::string ReceiveMessage() override;
};

} // namespace transport
} // namespace jsonrpc
