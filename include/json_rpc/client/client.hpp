#pragma once

#include <memory>
#include <string>

#include "json_rpc/core/client_transport.hpp"
#include "json_rpc/core/request.hpp"
#include "json_rpc/core/response.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

class Client {
public:
  Client(std::unique_ptr<ClientTransport> transport);
  ~Client() = default;

  // Send an RPC method to the server.
  Json SendMethodCall(const std::string &method, const Json &params, int id);

  // Send an RPC notification to the server.
  void SendNotification(const std::string &method, const Json &params);

private:
  std::unique_ptr<ClientTransport> transport_;
};

} // namespace json_rpc
