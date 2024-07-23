#pragma once

#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief An abstract class representing the transport mechanism for JSON-RPC
 * client.
 *
 * This class defines the interface for sending JSON-RPC method calls and
 * notifications. Concrete implementations can use various transport mechanisms
 * such as stdio, HTTP, sockets, etc.
 */
class ClientTransport {
public:
  /**
   * @brief Virtual destructor for ClientTransport.
   */
  virtual ~ClientTransport() = default;

  /**
   * @brief Send an RPC method call to the server.
   *
   * @param request The JSON-RPC method call request as a JSON object.
   * @return The response from the server as a JSON object.
   */
  virtual Json SendMethodCall(const Json &request) = 0;

  /**
   * @brief Send an RPC notification to the server.
   *
   * @param notification The JSON-RPC notification as a JSON object.
   */
  virtual void SendNotification(const Json &notification) = 0;
};

} // namespace json_rpc
