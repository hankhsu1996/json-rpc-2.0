#pragma once

#include <atomic>
#include <cassert>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "jsonrpc/client/request.hpp"
#include "jsonrpc/transport/transport.hpp"

namespace jsonrpc {
namespace client {

/**
 * @brief A JSON-RPC client for sending requests and receiving responses.
 *
 * This client allows for synchronous and asynchronous method calls as well as
 * notifications.
 */
class Client {
public:
  /**
   * @brief Constructs a JSON-RPC client with a specified transport layer.
   *
   * @param transport A unique pointer to a transport layer used for
   * communication.
   */
  Client(std::unique_ptr<transport::Transport> transport);

  /// @brief Destructor.
  ~Client() = default;

  /**
   * @brief Starts the JSON-RPC client listener thread.
   *
   * Initializes and starts a background thread that listens for responses from
   * the server. This must be called before sending any requests.
   */
  void Start();

  /**
   * @brief Stops the JSON-RPC client listener thread.
   *
   * Stops the background listener thread and waits for it to join.
   * This should be called before the client is destroyed or when the client no
   * longer needs to listen for responses.
   */
  void Stop();

  /**
   * @brief Checks if the client listener is running.
   *
   * @return True if the listener thread is running, false otherwise.
   */
  bool IsRunning() const;

  /**
   * @brief Sends a JSON-RPC method call and waits for the response.
   *
   * This is a blocking call that sends a method request to the server and waits
   * for the corresponding response.
   *
   * @param method The name of the method to call.
   * @param params Optional parameters to pass to the method.
   * @return The JSON response received from the server.
   */
  nlohmann::json SendMethodCall(const std::string &method,
      std::optional<nlohmann::json> params = std::nullopt);

  /**
   * @brief Sends a JSON-RPC method call asynchronously.
   *
   * This method sends a request to the server without blocking the calling
   * thread.
   *
   * @param method The name of the method to call.
   * @param params Optional parameters to pass to the method.
   * @return A future that will hold the JSON response from the server.
   */
  std::future<nlohmann::json> SendMethodCallAsync(const std::string &method,
      std::optional<nlohmann::json> params = std::nullopt);

  /**
   * @brief Sends a JSON-RPC notification.
   *
   * Notifications do not expect a response from the server.
   *
   * @param method The name of the method to notify.
   * @param params Optional parameters to pass to the method.
   */
  void SendNotification(const std::string &method,
      std::optional<nlohmann::json> params = std::nullopt);

  /**
   * @brief Checks if there are any pending requests.
   *
   * @return True if there are pending requests, false otherwise.
   */
  bool HasPendingRequests() const;

private:
  /// @brief Listener thread function for receiving responses from the transport
  /// layer.
  void Listener();

  /**
   * @brief Helper function to send a request and wait for a response.
   *
   * This function sends a JSON-RPC method call and waits for the response
   * synchronously. It wraps the asynchronous request handling with
   * blocking logic to return the result.
   *
   * @param request The JSON-RPC request to be sent.
   * @return The JSON response received from the server.
   */
  nlohmann::json SendRequest(const Request &request);

  /**
   * @brief Sends a request asynchronously.
   *
   * This method handles the logic for sending asynchronous requests.
   *
   * @param request The JSON-RPC request to be sent.
   * @return A future that will hold the JSON response from the server.
   */
  std::future<nlohmann::json> SendRequestAsync(const Request &request);

  /**
   * @brief Generates the next unique request ID.
   *
   * @return An integer representing the next unique request ID.
   */
  int GetNextRequestId();

  /**
   * @brief Handles a JSON-RPC response received from the transport layer.
   *
   * Parses the response and resolves the associated promise with the response
   * data.
   *
   * @param response The JSON-RPC response as a string.
   */
  void HandleResponse(const std::string &response);

  /**
   * @brief Validates a JSON-RPC response.
   *
   * Ensures that the response conforms to the JSON-RPC 2.0 specification.
   *
   * @param response The JSON-RPC response as a JSON object.
   * @return True if the response is valid, false otherwise.
   */
  bool ValidateResponse(const nlohmann::json &response);

  /// Transport layer for communication.
  std::unique_ptr<transport::Transport> transport_;

  /// Counter for generating unique request IDs.
  std::atomic<int> requestCounter_{0};

  /// Counter for tracking expected responses.
  std::atomic<int> expectedResponses_{0};

  /// Map of pending requests and their associated promises.
  std::unordered_map<int, std::promise<nlohmann::json>> pendingRequests_;

  /// Mutex to protect access to the pending requests map.
  mutable std::mutex pendingRequestsMutex_;

  /// Listener thread for receiving responses.
  std::thread listenerThread_;

  /// Flag to control the running state of the listener thread.
  std::atomic<bool> running_{false};
};

} // namespace client
} // namespace jsonrpc
