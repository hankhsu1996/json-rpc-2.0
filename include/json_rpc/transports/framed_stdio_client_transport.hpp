#pragma once

#include <string>

#include "json_rpc/core/client_transport.hpp"
#include "json_rpc/core/types.hpp"
#include "json_rpc/transports/framed_transport.hpp"

namespace json_rpc {

class FramedStdioClientTransport :
    public ClientTransport,
    protected FramedTransport {
public:
  Json SendMethodCall(const Json &request) override;
  void SendNotification(const Json &notification) override;
};

} // namespace json_rpc
