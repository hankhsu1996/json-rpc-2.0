#pragma once

#include <memory>
#include <string>

#include "json_rpc/core/client_transport.hpp"
#include "json_rpc/core/request.hpp"
#include "json_rpc/core/response.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief A class representing a JSON-RPC client.
 *
 * This class is used to send JSON-RPC method calls and notifications to a
 * server. The JSON-RPC library is transport agnostic, supporting transport
 * layers such as stdio, sockets, and pipes.
 */
class Client {
public:
  /**
   * @brief Constructs a new Client object.
   *
   * @param transport A unique pointer to a ClientTransport object used for
   * communication.
   */
  Client(std::unique_ptr<ClientTransport> transport);

  /**
   * @brief Destructor for Client.
   */
  ~Client() = default;

  /**
   * @brief Sends an RPC method call to the server.
   *
   * @param method The name of the method to be called.
   * @param params The parameters for the method call as a JSON object.
   * @param id The ID of the method call.
   * @return The response from the server as a JSON object.
   */
  Json SendMethodCall(const std::string &method, const Json &params, int id);

  /**
   * @brief Sends an RPC notification to the server.
   *
   * Notifications do not expect a response from the server.
   *
   * @param method The name of the method to be notified.
   * @param params The parameters for the notification as a JSON object.
   */
  void SendNotification(const std::string &method, const Json &params);

private:
  /**
   * @brief Transport mechanism for communication.
   */
  std::unique_ptr<ClientTransport> transport_;
};

} // namespace json_rpc
