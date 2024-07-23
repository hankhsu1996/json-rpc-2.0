#pragma once

#include <memory>
#include <string>

#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"

namespace json_rpc {

/**
 * @brief A JSON-RPC server that handles requests and notifications.
 *
 * The Server class manages the lifecycle of the JSON-RPC server, including
 * starting and stopping the server, and registering RPC methods and
 * notifications. It uses a Dispatcher to route incoming requests to the
 * appropriate handlers and a ServerTransport for communication.
 */
class Server {
public:
  /**
   * @brief Constructs a Server object with the specified transport.
   *
   * @param transport A unique pointer to the ServerTransport to be used for
   * communication.
   */
  Server(std::unique_ptr<ServerTransport> transport);

  /**
   * @brief Starts the server to begin handling requests.
   */
  void Start();

  /**
   * @brief Stops the server to stop handling requests.
   */
  void Stop();

  /**
   * @brief Registers an RPC method with the dispatcher.
   *
   * @param method The name of the RPC method.
   * @param handler The handler function to be called for this method.
   */
  void RegisterMethodCall(
      const std::string &method, const MethodCallHandler &handler);

  /**
   * @brief Registers an RPC notification with the dispatcher.
   *
   * @param method The name of the RPC notification.
   * @param handler The handler function to be called for this notification.
   */
  void RegisterNotification(
      const std::string &method, const NotificationHandler &handler);

  /**
   * @brief Checks if the server is running.
   *
   * @return True if the server is running, false otherwise.
   */
  bool IsRunning() const {
    return transport_->IsRunning();
  }

private:
  /// @brief Unique pointer to the Dispatcher.
  std::unique_ptr<Dispatcher> dispatcher_;

  /// @brief Unique pointer to the ServerTransport.
  std::unique_ptr<ServerTransport> transport_;
};

} // namespace json_rpc
