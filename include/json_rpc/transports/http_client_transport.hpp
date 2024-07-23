#pragma once

#include <httplib.h>
#include <string>

#include "json_rpc/core/client_transport.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief Client transport using HTTP for communication.
 *
 * This class implements the ClientTransport interface using HTTP
 * as the underlying transport mechanism.
 */
class HttpClientTransport : public ClientTransport {
public:
  /**
   * @brief Constructs an HttpClientTransport object.
   *
   * @param server_url The URL of the server.
   */
  explicit HttpClientTransport(const std::string &server_url);

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

private:
  /// @brief The URL of the server.
  std::string server_url_;

  /// @brief The HTTP client used for communication.
  httplib::Client cli_;
};

} // namespace json_rpc
