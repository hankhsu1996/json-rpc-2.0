#pragma once

#include <memory>
#include <string>

#include "jsonrpc/core/dispatcher.hpp"
#include "jsonrpc/core/server_transport.hpp"
#include "jsonrpc/core/types.hpp"

namespace jsonrpc {

/**
 * @brief A JSON-RPC server for handling requests and notifications.
 *
 * Manages the lifecycle of a JSON-RPC server, including starting and stopping,
 * and registering RPC methods and notifications.
 */
class Server {
public:
  /**
   * @brief Constructs a Server with the specified transport.
   *
   * @param transport A unique pointer to the ServerTransport for communication.
   */
  Server(std::unique_ptr<ServerTransport> transport);

  /// @brief Starts the server to handle incoming JSON-RPC requests.
  void Start();

  /// @brief Stops the server from handling requests.
  void Stop();

  /**
   * @brief Registers an RPC method handler with the dispatcher.
   *
   * @param method The name of the RPC method to handle.
   * @param handler The function to handle the RPC method call.
   *
   * @see MethodCallHandler for the signature of the handler function.
   */
  void RegisterMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  /**
   * @brief Registers an RPC notification handler with the dispatcher.
   *
   * @param method The name of the RPC notification to handle.
   * @param handler The function to handle the RPC notification.
   *
   * @see NotificationHandler for the signature of the handler function.
   */
  void RegisterNotification(
      const std::string &method, const NotificationHandler &handler);

  /// @brief Checks if the server is currently running.
  bool IsRunning() const;

private:
  /// @brief Dispatcher for routing requests to the appropriate handlers.
  std::shared_ptr<Dispatcher> dispatcher_;

  /// @brief Transport layer for communication.
  std::unique_ptr<ServerTransport> transport_;
};

} // namespace jsonrpc
