#pragma once

#include <iostream>

#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/server_transport.h"
#include "json_rpc/core/types.h"

namespace json_rpc {

class StdioServerTransport : public ServerTransport {
protected:
  void Listen() override;
};

} // namespace json_rpc
