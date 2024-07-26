#pragma once

#include <string>

#include <httplib/httplib.h>

#include "jsonrpc/core/dispatcher.hpp"
#include "jsonrpc/core/server_transport.hpp"
#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

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

} // namespace jsonrpc
