#pragma once

#include <string>

#include "json_rpc/core/transport.h"
#include "json_rpc/core/types.h"
#include "json_rpc/transports/framed_transport.h"

namespace json_rpc {

class FramedStdioClientTransport :
    public ClientTransport,
    protected FramedTransport {
public:
  Json SendMethodCall(const Json &request) override;
  void SendNotification(const Json &notification) override;
};

class FramedStdioServerTransport :
    public ServerTransport,
    protected FramedTransport {
protected:
  void Listen() override;
};

} // namespace json_rpc
