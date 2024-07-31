#pragma once

#include <string>

#include "jsonrpc/core/client_transport.hpp"

namespace jsonrpc {

class StdioClientTransport : public ClientTransport {
public:
  void SendRequest(const std::string &request) override;
  std::string ReadResponse() override;
};

} // namespace jsonrpc
