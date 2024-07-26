#pragma once

#include <string>

#include "jsonrpc/core/server_transport.hpp"
#include "jsonrpc/core/types.hpp"
#include "jsonrpc/transports/framed_transport.hpp"

namespace jsonrpc {

/**
 * @brief Server transport using framed stdio for communication.
 *
 * This class implements the ServerTransport interface using framed stdio
 * as the underlying transport mechanism.
 */
class FramedStdioServerTransport :
    public ServerTransport,
    protected FramedTransport {
protected:
  /**
   * @brief Listens for incoming messages and dispatches them to the appropriate
   * handlers.
   */
  void Listen() override;
};

} // namespace jsonrpc
