#pragma once

#include <string>

#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"
#include "json_rpc/transports/framed_transport.hpp"

namespace json_rpc {

class FramedStdioServerTransport :
    public ServerTransport,
    protected FramedTransport {
protected:
  void Listen() override;
};

} // namespace json_rpc
