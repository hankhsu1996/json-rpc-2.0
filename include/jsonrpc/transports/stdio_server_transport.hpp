#pragma once

#include <iostream>

#include "jsonrpc/core/dispatcher.hpp"
#include "jsonrpc/core/server_transport.hpp"
#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

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

} // namespace jsonrpc
