#pragma once

#include <iostream>

#include "json_rpc/core/client_transport.hpp"
#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

class StdioClientTransport : public ClientTransport {
public:
  Json SendMethodCall(const Json &request) override;
  void SendNotification(const Json &notification) override;
};

} // namespace json_rpc
