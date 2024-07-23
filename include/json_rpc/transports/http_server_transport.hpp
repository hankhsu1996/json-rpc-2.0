#pragma once

#include <httplib.h>
#include <string>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief Server transport using HTTP for communication.
 *
 * This class implements the ServerTransport interface using HTTP
 * as the underlying transport mechanism.
 */
class HttpServerTransport : public ServerTransport {
public:
  /**
   * @brief Constructs an HttpServerTransport object.
   *
   * @param host The host address of the server.
   * @param port The port number on which the server will listen.
   */
  HttpServerTransport(const std::string &host, int port);

  /**
   * @brief Listens for incoming HTTP requests and dispatches them to the
   * appropriate handlers.
   */
  void Listen() override;

  /**
   * @brief Stops the server.
   */
  void Stop();

private:
  /// @brief The host address of the server.
  std::string host_;

  /// @brief  The port number on which the server listens.
  int port_;

  /// @brief The HTTP server used for communication.
  httplib::Server svr_;
};

} // namespace json_rpc
