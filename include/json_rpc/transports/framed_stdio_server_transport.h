#pragma once

#include <string>

#include "json_rpc/core/server_transport.h"
#include "json_rpc/core/types.h"
#include "json_rpc/transports/framed_transport.h"

namespace json_rpc {

class FramedStdioServerTransport :
    public ServerTransport,
    protected FramedTransport {
protected:
  void Listen() override;
};

} // namespace json_rpc
