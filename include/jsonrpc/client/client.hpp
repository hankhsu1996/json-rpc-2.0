#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "jsonrpc/client/request.hpp"
#include "jsonrpc/core/client_transport.hpp"

namespace jsonrpc {

/// A JSON-RPC client for sending requests and receiving responses.
class Client {
public:
  /**
   * @brief Constructs a JSON-RPC client with a specified transport layer.
   * @param transport A unique pointer to a transport layer used for
   * communication.
   */
  Client(std::unique_ptr<ClientTransport> transport);
  ~Client() = default;

  /**
   * @brief Starts the JSON-RPC client listener thread.
   *
   * This function initializes and starts a background thread that listens
   * for responses from the server. It must be called before sending any
   * requests.
   */
  void Start();

  /**
   * @brief Stops the JSON-RPC client listener thread.
   *
   * This function stops the background listener thread and waits for it to
   * join. It should be called before the client is destroyed or when the client
   * no longer needs to listen for responses.
   */
  void Stop();

  /**
   * @brief Checks if the client listener is running.
   * @return True if the listener thread is running, false otherwise.
   */
  bool isRunning() const;

  /**
   * @brief Sends a JSON-RPC method call and waits for the response.
   * @param method The name of the method to call.
   * @param params Optional parameters to pass to the method.
   * @return The JSON response received from the server.
   *
   * This function sends a JSON-RPC method call to the server and waits
   * for the corresponding response. The response is returned as a JSON object.
   */
  Json SendMethodCall(
      const std::string &method, std::optional<Json> params = std::nullopt);

  /**
   * @brief Sends a JSON-RPC notification.
   * @param method The name of the method to notify.
   * @param params Optional parameters to pass to the method.
   *
   * This function sends a JSON-RPC notification to the server. Unlike method
   * calls, notifications do not expect a response from the server.
   */
  void SendNotification(
      const std::string &method, std::optional<Json> params = std::nullopt);

private:
  /// Listener thread function for receiving responses from the transport layer.
  void Listener();

  /// Helper function to send a request and optionally wait for a response.
  Json SendRequest(const ClientRequest &request);

  /// Generates the next unique request ID.
  int GetNextRequestId();

  /// Handles a JSON-RPC response received from the transport layer.
  void HandleResponse(const std::string &response);

  /// Validates a JSON-RPC response.
  bool ValidateResponse(const Json &response);

  /// Transport layer for communication.
  std::unique_ptr<ClientTransport> transport_;

  /// Counter for generating unique request IDs.
  std::atomic<int> requestCounter_{0};

  /// Counter for tracking expected responses.
  std::atomic<int> expectedResponses_{0};

  /// Map of pending requests and their associated promises.
  std::unordered_map<int, std::promise<Json>> pendingRequests_;

  /// Mutex to protect access to the pending requests map.
  std::mutex pendingRequestsMutex_;

  /// Listener thread for receiving responses.
  std::thread listenerThread_;

  /// Flag to control the running state of the listener thread.
  std::atomic<bool> running_{true};
};

} // namespace jsonrpc
