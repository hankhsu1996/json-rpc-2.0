#pragma once

#include <iostream>

#include "jsonrpc/server/dispatcher.hpp"
#include "jsonrpc/server/transports/server_transport.hpp"

namespace jsonrpc {
namespace server {
namespace transports {

/**
 * @brief Server transport using stdio for communication.
 *
 * This class implements the ServerTransport interface using stdio
 * as the underlying transport mechanism.
 */
class StdioServerTransport : public ServerTransport {
protected:
  /**
   * @brief Listens for incoming messages and dispatches them to the appropriate
   * handlers.
   */
  void Listen() override;
};

} // namespace transports
} // namespace server
} // namespace jsonrpc
