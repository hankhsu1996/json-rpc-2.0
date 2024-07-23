#pragma once

#include <iostream>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

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

} // namespace json_rpc
