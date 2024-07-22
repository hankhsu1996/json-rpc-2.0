#pragma once

#include <iostream>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

class StdioServerTransport : public ServerTransport {
protected:
  void Listen() override;
};

} // namespace json_rpc
