#pragma once

#include <string>

#include "jsonrpc/client/transports/client_transport.hpp"

namespace jsonrpc {
namespace client {
namespace transports {

/**
 * @brief Transport layer using standard I/O for JSON-RPC communication.
 */
class StdioClientTransport : public ClientTransport {
public:
  void SendRequest(const std::string &request) override;
  std::string ReadResponse() override;
};

} // namespace transports
} // namespace client
} // namespace jsonrpc
