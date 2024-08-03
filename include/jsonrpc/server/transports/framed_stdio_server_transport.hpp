#pragma once

#include <string>

#include "jsonrpc/common/transports/framed_transport.hpp"
#include "jsonrpc/server/transports/server_transport.hpp"

namespace jsonrpc {
namespace server {
namespace transports {

/**
 * @brief Server transport using framed stdio for communication.
 *
 * This class implements the ServerTransport interface using framed stdio
 * as the underlying transport mechanism.
 */
class FramedStdioServerTransport :
    public ServerTransport,
    protected common::transports::FramedTransport {
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
