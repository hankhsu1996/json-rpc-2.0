#pragma once

#include <string>

#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"
#include "json_rpc/transports/framed_transport.hpp"

namespace json_rpc {

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
   * @brief Listens for incoming messages and dispatches them to the appropriate handlers.
   */
  void Listen() override;
};

} // namespace json_rpc
