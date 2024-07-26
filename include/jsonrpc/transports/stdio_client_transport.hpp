#pragma once

#include <iostream>

#include "jsonrpc/core/client_transport.hpp"
#include "jsonrpc/core/dispatcher.hpp"
#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

/**
 * @brief Client transport using stdio for communication.
 *
 * This class implements the ClientTransport interface using stdio
 * as the underlying transport mechanism.
 */
class StdioClientTransport : public ClientTransport {
public:
  /**
   * @brief Sends an RPC method call to the server.
   *
   * @param request The JSON-RPC request as a JSON object.
   * @return The response from the server as a JSON object.
   */
  Json SendMethodCall(const Json &request) override;

  /**
   * @brief Sends an RPC notification to the server.
   *
   * @param notification The JSON-RPC notification as a JSON object.
   */
  void SendNotification(const Json &notification) override;
};

} // namespace jsonrpc
