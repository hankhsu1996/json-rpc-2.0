#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "jsonrpc/client/request.hpp"
#include "jsonrpc/core/client_transport.hpp"

namespace jsonrpc {
/**
 * @brief A JSON-RPC client for sending requests and receiving responses.
 */
class Client {
public:
  /**
   * @brief Constructs a JSON-RPC client with a specified transport layer.
   * @param transport A unique pointer to a transport layer.
   */
  Client(std::unique_ptr<ClientTransport> transport);
  ~Client() = default;

  /**
   * @brief Starts the JSON-RPC client listener thread.
   */
  void Start();

  /**
   * @brief Stops the JSON-RPC client listener thread.
   */
  void Stop();

  // Sends a method call and returns the response
  nlohmann::json SendMethodCall(const std::string &method,
      std::optional<nlohmann::json> params = std::nullopt);

  // Sends a notification (no response expected)
  void SendNotification(const std::string &method,
      std::optional<nlohmann::json> params = std::nullopt);

private:
  /**
   * @brief Listener thread function for receiving responses from the transport
   * layer.
   */
  void Listener();

  // Helper function to send a request and optionally wait for a response
  nlohmann::json SendRequest(const ClientRequest &request);

  /**
   * @brief Generates the next unique request ID.
   * @return The next unique request ID.
   */
  int GetNextRequestId();

  /**
   * @brief Handles a JSON-RPC response received from the transport layer.
   * @param response The JSON-RPC response as a string.
   */
  void HandleResponse(const std::string &response);

  /**
   * @brief Validates a JSON-RPC response.
   * @param response The JSON-RPC response to validate.
   * @return True if the response is valid, false otherwise.
   */
  bool ValidateResponse(const nlohmann::json &response);

  /// Transport layer for communication.
  std::unique_ptr<ClientTransport> transport_;

  /// Counter for generating unique request IDs.
  std::atomic<int> requestCounter_{0};

  /// Counter for tracking expected responses.
  std::atomic<int> expectedResponses_{0};

  /// Map of pending requests and their associated promises.
  std::unordered_map<int, std::promise<nlohmann::json>> pendingRequests_;

  /// Mutex to protect access to the pending requests map.
  std::mutex pendingRequestsMutex_;

  /// Listener thread for receiving responses.
  std::thread listenerThread_;

  /// Flag to control the running state of the listener thread.
  std::atomic<bool> running_{true};
};

} // namespace jsonrpc
