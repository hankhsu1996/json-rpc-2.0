#pragma once

#include <string>

#include "jsonrpc/core/client_transport.hpp"

namespace jsonrpc {

/**
 * @brief Transport layer using standard I/O for JSON-RPC communication.
 */
class StdioClientTransport : public ClientTransport {
public:
  void SendRequest(const std::string &request) override;
  std::string ReadResponse() override;
};

} // namespace jsonrpc
