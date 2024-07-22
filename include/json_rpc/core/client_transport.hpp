#pragma once

#include "json_rpc/core/types.hpp"

namespace json_rpc {

class ClientTransport {
public:
  virtual ~ClientTransport() = default;

  // Send an RPC method to the server.
  virtual Json SendMethodCall(const Json &request) = 0;

  // Send an RPC notification to the server.
  virtual void SendNotification(const Json &notification) = 0;
};

} // namespace json_rpc
